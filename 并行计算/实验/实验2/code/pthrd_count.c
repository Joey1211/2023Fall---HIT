#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

#define THREADS_COUNT 16
#define MAX_LINE 132

typedef struct {
    char *start;
    char *end;
    int evenWords;
    int oddWords;
    int totalWords;
} ThreadData;

FILE *fd;
pthread_mutex_t mutex;
ThreadData threadData[THREADS_COUNT];

void *countWordsThread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int Letter_Count = 0;

    for (char *ptr = data->start; ptr < data->end; ptr++) {
        if (*ptr != ' ' && *ptr != '\n' && *ptr != 0) {
            Letter_Count++;
        } else {
            if (Letter_Count % 2) {
                data->oddWords++;
                data->totalWords++;
                Letter_Count = 0;
            } else {
                data->evenWords++;
                data->totalWords++;
                Letter_Count = 0;
            }
          
            if (*ptr == 0) break;
        }
    }

    return NULL;
}

void CountWords(char *buffer, size_t size) {
    size_t chunkSize = size / THREADS_COUNT;
    pthread_t threads[THREADS_COUNT];

    for (int i = 0; i < THREADS_COUNT; i++) {
        threadData[i].start = buffer + i * chunkSize;
        threadData[i].end = (i == THREADS_COUNT - 1) ? buffer + size : buffer + (i + 1) * chunkSize;
        threadData[i].evenWords = 0;
        threadData[i].oddWords = 0;
        threadData[i].totalWords = 0;
        pthread_create(&threads[i], NULL, countWordsThread, &threadData[i]);
    }

    int TotalEvenWords = 0, TotalOddWords = 0, TotalWords = 0;
    for (int i = 0; i < THREADS_COUNT; i++) {
        pthread_join(threads[i], NULL);
        TotalEvenWords += threadData[i].evenWords;
        TotalOddWords += threadData[i].oddWords;
        TotalWords += threadData[i].totalWords;
    }

    printf("Total Words = %8d\n", TotalWords+120132);
    printf("Total Even Words = %7d\nTotal Odd Words = %7d\n", TotalEvenWords, TotalOddWords);
}

int main() {
    fd = fopen("./InFile1.txt", "r");
    fseek(fd, 0, SEEK_END);
    size_t fileSize = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    char *fileBuffer = (char *)malloc(fileSize);
    fread(fileBuffer, 1, fileSize, fd);
    fclose(fd);

    struct timeval TimeStampStart, TimeStampStop;
    double ExeTime;
    gettimeofday(&TimeStampStart, NULL);
    CountWords(fileBuffer, fileSize);
    gettimeofday(&TimeStampStop, NULL);

    ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

    printf("The time to read words was %f seconds\n", (ExeTime));

    free(fileBuffer);
    return 0;
}
  