#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

#define BLOCK_SIZE 500

int main(){
    struct timeval TimeStampStart, TimeStampStop;
    double ExeTime;
    unsigned int iter=200000000; 
    int i, j;
    double x, y;
    double dUnderCurve=0.0;
    double pi=0.0;

    gettimeofday(&TimeStampStart, NULL);

    #pragma omp parallel private(i, j, x, y) 
    {
        double r[BLOCK_SIZE*2];
        unsigned int seed = time(NULL) ^ omp_get_thread_num(); // Different seed for each thread

        #pragma omp for reduction(+:dUnderCurve)
        for(j=0; j<iter/BLOCK_SIZE; j++) {
            // Create random numbers into array r
            for (i=0; i<BLOCK_SIZE*2; i++) {
                r[i] = 0.0 + 1.0 * rand_r(&seed) / RAND_MAX * ( 1.0 - 0.0 );
            }
        
            for (i=0; i<BLOCK_SIZE; i++) {
                x=r[i];
                y=r[i+BLOCK_SIZE];
                if (x*x + y*y <= 1.0) {
                    dUnderCurve++;
                }
            } 
        }
    }

    pi = dUnderCurve / (double) iter * 4;

    gettimeofday(&TimeStampStop, NULL);
    ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

    printf ("pi = %10.9f\n", pi);
    printf("The time to calculate PI was %f seconds\n", (ExeTime));
    
    return 0;
}
