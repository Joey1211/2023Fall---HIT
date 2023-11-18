#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>

long long num_steps = 1000000000;
double step;
double sum=0.0;
double  pi;

//long NumThreads
pthread_mutex_t mutex;
const int NumThreads = 10;


/*Parallel function*/
static void* Thread_sum(void* rank) {
    int my_rank = *((int*)rank);

    long long i;
    long long my_n = num_steps/NumThreads;
    long long my_first_i = my_n * my_rank;
    long long my_last_i = my_first_i + my_n;
    double my_sum = 0.0;

    double x;

    for(i = my_first_i; i < my_last_i; i++) {
        x = (i + .5)*step;
		my_sum = my_sum + 4.0/(1.+ x*x);
    }
    pthread_mutex_lock(&mutex);
    sum += my_sum;
    pthread_mutex_unlock(&mutex);
}/*Thread_sum function*/


int main(int argc, char* argv[])
{
	struct timeval TimeStampStart, TimeStampStop;
	double ExeTime;

	int i;
	//int NumThreads;
	

	/*printf("Input NumThreads:");
	scanf("%ld\n",&NumThreads);*/
	pthread_mutex_init(&mutex, NULL);

	step = 1./(double)num_steps;

	int Num[NumThreads];
	pthread_t ThreadIDs[NumThreads];
    pthread_attr_t attr[NumThreads];
    gettimeofday(&TimeStampStart, NULL);
	for (int i = 0; i < NumThreads; i++) {
		Num[i] = i;
		pthread_attr_init(&attr[i]);
		pthread_attr_setdetachstate(&attr[i], PTHREAD_CREATE_JOINABLE);
	}

	for (int i = 0; i < NumThreads; i++) {
		int err = pthread_create(&ThreadIDs[i], &attr[i], Thread_sum, (void*)&Num[i]);
		if(err != 0) {
			printf("ERROR: pthread_create() return code: %d\n", err);
        }
	}

	for (int i = 0; i < NumThreads; i++) {
		pthread_join(ThreadIDs[i], NULL);
	//	printf("Thread %d end !\n", i);
	}
	
	pi = sum*step;
	gettimeofday(&TimeStampStop, NULL);
    ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

	printf("The value of PI is %15.12f\n",pi);
	printf("The time to calculate PI was %f seconds\n", (ExeTime));
	pthread_mutex_destroy(&mutex);
	return 0;
}
