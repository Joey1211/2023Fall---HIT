#include <stdio.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 4

long long num_steps = 1000000000;
double step;
double sum[NUM_THREADS];
pthread_mutex_t mutex;

void* CalculatePi(void* arg)
{
    int threadId = *((int*)arg);
    double x, partialSum = 0.0;

    for (int i = threadId; i < num_steps; i += NUM_THREADS) {
        x = (i + 0.5) * step;
        partialSum += 4.0 / (1.0 + x * x);
    }

    pthread_mutex_lock(&mutex);
    sum[threadId] = partialSum;
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    struct timeval TimeStampStart, TimeStampStop;
    double ExeTime;
    double pi = 0.0;
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    step = 1.0 / (double)num_steps;

    gettimeofday(&TimeStampStart, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        int* threadId = (int*)malloc(sizeof(int));
        *threadId = i;
        pthread_create(&threads[i], NULL, CalculatePi, (void*)threadId);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        pi += sum[i];
    }

    pi *= step;

    gettimeofday(&TimeStampStop, NULL);
    ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

    printf("The value of PI is %15.12f\n", pi);
    printf("The time to calculate PI was %f seconds\n", ExeTime);

    pthread_mutex_destroy(&mutex);

    return 0;
}


