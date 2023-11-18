#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>

long long num_steps = 1000000000;
double step;
double sum;
pthread_mutex_t mut;

void* thread_sum();

int main(int argc, char* argv[])
{
	pthread_mutex_init(&mut,NULL);  //互斥量初始化
	
	struct timeval TimeStampStart, TimeStampStop;
	double ExeTime;
	sum = 0.0;
	double pi;
	step = 1./(double)num_steps;
	gettimeofday(&TimeStampStart, NULL);
	
	thread_sum();

	pi = sum*step;
	gettimeofday(&TimeStampStop, NULL);
    ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

	printf("The value of PI is %15.12f\n",pi);
	printf("The time to calculate PI was %f seconds\n", (ExeTime));
	
	pthread_mutex_destroy(&mut); 	// 销毁互斥量
	
	return 0;
}

void* thread_sum(){
	double x,mysum = 0.0;
	int i;
	
	for (i=0; i<num_steps; i++)
	{
		x = (i + .5)*step;
		mysum += 4.0/(1.+ x*x);
	}
	
	pthread_mutex_lock(&mut);
	sum += mysum;
	pthread_mutex_unlock(&mut);

}

