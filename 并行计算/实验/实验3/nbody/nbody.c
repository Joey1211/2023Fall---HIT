#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory.h>

#define drand48() (double)rand()/RAND_MAX

/* Pipeline version of the algorithm... */
/* we really need the velocities as well... */
typedef struct {
	double x, y, z;
	double mass;
} Particle;
/* We use leapfrog for the time integration ... */
typedef struct {
	double xold, yold, zold;
	double fx, fy, fz;
} ParticleV;


void InitParticles( Particle[], ParticleV [], int );
double ComputeForces( Particle [],  ParticleV [], int, Particle [], int);
double ComputeNewPos( Particle [], ParticleV [], int, double, MPI_Comm );


#define MAX_PARTICLES 11000
#define MAX_P          128
	int
main( int argc, char *argv[] )
{
	Particle	particles[MAX_PARTICLES];   /* Particles on ALL nodes */
	ParticleV	pv[MAX_PARTICLES];          /* Particle velocity */
	Particle	*globalbuf;                 /* global buffers */
	int         counts[MAX_P],              /* Number on each processor */
		    displs[MAX_P];					/* Offsets into particles */
	int         rank, size, npart, i, k,
		    offset;							/* location of local particles */
	int         totpart,                    /* total number of particles */
		    cnt, allparticlescount;			/* number of times in loop */
	MPI_Datatype particletype;
	double      sim_t;                      /* Simulation time */
	double      time, time1;				/* Computation time */
	double 	computationTime = 0;
	int         pipe;
	MPI_Status  statuses[2];

	char        processor_name[MPI_MAX_PROCESSOR_NAME];
	int         namelen;

	int         ringrank;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );


	ringrank = rank + size;

	/* Everyone COULD have a different size ... */
	if (argc < 3) { 
		fprintf( stderr, "Usage: %s n localsize\n", argv[0] );
		MPI_Abort( MPI_COMM_WORLD, 1 );
	}
	npart = atoi(argv[2]);
	allparticlescount = atoi(argv[1]);

	if (npart > MAX_PARTICLES) {
		fprintf( stderr, "%d is too many for process %d; max is %d\n", 
				npart, rank, MAX_PARTICLES );
		MPI_Abort( MPI_COMM_WORLD, 1 );
	}

	time = MPI_Wtime();

	MPI_Type_contiguous( 4, MPI_DOUBLE, &particletype );
	MPI_Type_commit( &particletype );

	/* Get the sizes and displacements */
	MPI_Allgather( &npart, 1, MPI_INT, counts, 1, MPI_INT, MPI_COMM_WORLD );
	displs[0] = 0;
	for (i=1; i<size; i++) 
		displs[i] = displs[i-1] + counts[i-1];
	totpart = displs[size-1] + counts[size-1];

	if (rank == 0){
		printf( "The number of bodies requested is %d, but the number of bodies computed actually is %d\n", atoi(argv[1]), totpart);
	}      

	/* Generate the initial values */
	InitParticles( particles, pv, npart);
	offset = displs[rank];
	cnt    = 10;

	sim_t = 0.0;

	if ((globalbuf = (Particle*)calloc(allparticlescount, sizeof(Particle))) == NULL) {
		exit(1);
	}

	while (cnt--) {
		double max_f, max_f_seg;
		k = 0;
		/* Load the initial sendbuffer */
		memcpy( globalbuf, particles, npart * sizeof(Particle) );
		max_f = 0.0;
		for (pipe=0; pipe<size - 1; pipe++) {
			MPI_Send( globalbuf + k, counts[(ringrank-pipe)%size], particletype, (rank+1)%size, pipe, 
					MPI_COMM_WORLD );
			k += counts[(ringrank-pipe)%size];
			MPI_Recv( globalbuf + k, counts[(ringrank-pipe-1)%size], particletype, (ringrank-1)%size,  pipe, MPI_COMM_WORLD, statuses );                        
		} 
		max_f_seg = ComputeForces( particles,  pv, npart, globalbuf, allparticlescount);
		if (max_f_seg > max_f) max_f = max_f_seg;
		/* Once we have the forces, we compute the changes in position */
		sim_t += ComputeNewPos( particles, pv, npart, max_f, MPI_COMM_WORLD );
		/* We could do graphics here (move particles on the display) */
	} 
	time = MPI_Wtime() - time;

	MPI_Get_processor_name(processor_name, &namelen);
	printf( "%d bodys are assigned to rank %d, hostname is %s, Computation Time is %f\n", npart, rank, processor_name, time);

	MPI_Finalize();
	return 0;
} 

void InitParticles( Particle particles[], ParticleV pv[], int npart )
{
	int i;
	for (i=0; i<npart; i++) {
		particles[i].x	  = drand48();
		particles[i].y	  = drand48();
		particles[i].z	  = drand48();
		particles[i].mass = 1.0;
		pv[i].xold	  = particles[i].x;
		pv[i].yold	  = particles[i].y;
		pv[i].zold	  = particles[i].z;
		pv[i].fx	  = 0;
		pv[i].fy	  = 0;
		pv[i].fz	  = 0;
	}
}

double ComputeForces( Particle myparticles[], ParticleV pv[], int npart, Particle others[], int otherpart )
{
	double max_f, rmin;
	int i, j;

	max_f = 0.0;
	for (i=0; i<npart; i++) 
	{
		double xi, yi, zi, rx, ry, rz, mj, r, fx, fy, fz;
		rmin = 100.0;
		xi   = myparticles[i].x;
		yi   = myparticles[i].y;
		zi   = myparticles[i].z;
		fx   = 0.0;
		fy   = 0.0;
		fz   = 0.0;
		for (j=0; j<otherpart; j++) 
		{
			rx = xi - others[j].x;
			ry = yi - others[j].y;
			rz = zi - others[j].z;
			mj = others[j].mass;
			r  = rx * rx + ry * ry + rz*rz;
			/* ignore overlap and same particle */
			if (r == 0.0) continue;
			if (r < rmin) rmin = r;
			/* compute forces */
			r  = r * sqrt(r);
			fx -= mj * rx / r;
			fy -= mj * ry / r;
			fz -= mj * rz / r;
		}
		pv[i].fx += fx;
		pv[i].fy += fy;
		pv[i].fz += fz;
		/* Compute a rough estimate of (1/m)|df / dx| */
		fx		      = sqrt(fx*fx + fy*fy + fz*fz)/rmin;
		if (fx > max_f) max_f = fx;
	}
	return max_f;
}

double ComputeNewPos( Particle particles[], ParticleV pv[], int npart, 
		double max_f, MPI_Comm commring )
{
	int i;
	double      a0, a1, a2;
	static      double dt_old = 0.001, dt = 0.001;
	double      dt_est, dt_new;

	/* integation is a0 * x^+ + a1 * x + a2 * x^- = f / m */

	a0	 = 2.0 / (dt * (dt + dt_old));
	a2	 = 2.0 / (dt_old * (dt + dt_old));
	a1	 = -(a0 + a2);      /* also -2/(dt*dt_old) */

	for (i=0; i<npart; i++) {
		double xi, yi, zi;
		/* Very, very simple leapfrog time integration.  We use a variable 
		   step version to simplify time-step control. */
		xi	           = particles[i].x;
		yi	           = particles[i].y;
		zi		   = particles[i].z;
		/* x1-x0=s0, x2-x1=s1 dv/dt=f/m=f (s1/dt-s0/dt_old)*2/(dt+dt_old))=f */
		particles[i].x = (pv[i].fx - a1 * xi - a2 * pv[i].xold) / a0;
		particles[i].y = (pv[i].fy - a1 * yi - a2 * pv[i].yold) / a0;
		particles[i].z = (pv[i].fz - a1 * zi - a2 * pv[i].zold) / a0;
		pv[i].xold     = xi;
		pv[i].yold     = yi;
		pv[i].zold	   = zi;
		pv[i].fx       = 0;
		pv[i].fy       = 0;
		pv[i].fz	   = 0;
	}

	/* Recompute a time step. Stability criteria is roughly 
	   2/sqrt(1/m |df/dx|) >= dt.  We leave a little room */
	dt_est = 1.0/sqrt(max_f);
	/* Set a minimum: */
	if (dt_est < 1.0e-6) dt_est = 1.0e-6;
	MPI_Allreduce( &dt_est, &dt_new, 1, MPI_DOUBLE, MPI_MIN, commring );
	/* Modify time step */
	if (dt_new < dt) {
		dt_old = dt;
		dt     = dt_new;
	}
	else if (dt_new > 4.0 * dt) {
		dt_old = dt;
		dt    *= 2.0;
	}

	return dt_old;
}







