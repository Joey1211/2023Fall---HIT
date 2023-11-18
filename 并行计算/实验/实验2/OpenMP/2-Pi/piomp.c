#include <stdio.h>
#include <sys/time.h>
#include <omp.h>

long long num_steps = 1000000000;
double step;

int main(int argc, char* argv[])
{
    struct timeval TimeStampStart, TimeStampStop;
    double ExeTime;
	double x, pi, sum=0.0;
	int i;
	
	step = 1./(double)num_steps;
	gettimeofday(&TimeStampStart, NULL);
	
	#pragma omp parallel for private(x) reduction(+:sum)
	for (i=0; i<num_steps; i++)
	{
		x = (i + .5)*step;
		sum = sum + 4.0/(1.+ x*x);
	}
	
	pi = sum*step;
	gettimeofday(&TimeStampStop, NULL);
	ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

	printf("The value of PI is %15.12f\n",pi);
	printf("The time to calculate PI was %f seconds\n", (ExeTime));
	return 0;
}
