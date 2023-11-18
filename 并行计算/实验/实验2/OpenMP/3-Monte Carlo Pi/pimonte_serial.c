#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define BLOCK_SIZE 500

int main(){
    struct timeval TimeStampStart, TimeStampStop;
    double ExeTime;
    unsigned int iter=200000000; 
    int i, j;
    double x, y;
    double dUnderCurve=0.0;
    double pi=0.0;

    double r[BLOCK_SIZE*2];  //Careful!!! 
    //you need a private copy of whole array for each thread             
    srand((unsigned)time(NULL));
    gettimeofday(&TimeStampStart, NULL);

    for(j=0; j<iter/BLOCK_SIZE; j++) {
        // Create random numbers into array r
        // 生成 BLOCK_SIZE*2 个在 0.0-1.0 内均匀分布的随机数, 作为横纵坐标
        for (i=0; i<BLOCK_SIZE*2; i++) {
            r[i] = 0.0 + 1.0 * rand() / RAND_MAX * ( 1.0 - 0.0 );
        }
    
        for (i=0; i<BLOCK_SIZE; i++) {
            x=r[i];                     //X Coordinate
            y=r[i+BLOCK_SIZE];          //Y Coordinate
            if (x*x + y*y <= 1.0) {	//is distance from Origin under Curve 
                dUnderCurve++;
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
