#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <omp.h>

#define BLOCK_SIZE 500

int main() {
    struct timeval TimeStampStart, TimeStampStop;
    double ExeTime;
    unsigned int iter = 200000000;
    int i, j;
    double x, y;
    double dUnderCurve = 0.0;
    double pi = 0.0;

    unsigned int seed = (unsigned int)time(NULL);
    gettimeofday(&TimeStampStart, NULL);

    #pragma omp parallel private(x, y) reduction(+:dUnderCurve)
    {
        unsigned int private_seed = seed + omp_get_thread_num();

        #pragma omp for
        for (j = 0; j < iter / BLOCK_SIZE; j++) {
            for (i = 0; i < BLOCK_SIZE; i++) {
                x = (double)rand_r(&private_seed) / RAND_MAX;    // X Coordinate
                y = (double)rand_r(&private_seed) / RAND_MAX;    // Y Coordinate
                if (x * x + y * y <= 1.0) {    // is distance from Origin under Curve
                    dUnderCurve++;
                }
            }
        }
    }

    pi = 4 * dUnderCurve / (double)iter;

    gettimeofday(&TimeStampStop, NULL);
    ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

    printf("pi = %10.9f\n", pi);
    printf("The time to calculate PI was %f seconds\n", (ExeTime));

    return 0;
}

