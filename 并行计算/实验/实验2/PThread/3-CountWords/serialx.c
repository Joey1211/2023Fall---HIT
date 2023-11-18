#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

#define NUM_THREADS 2

FILE* fd;
int TotalEvenWords = 0, TotalOddWords = 0, TotalWords = 0;
pthread_mutex_t mutex;

int GetNextLine(FILE* f, char* Line)
{
    if (fgets(Line, 132, f) == NULL)
        if (feof(f))
            return EOF;
        else
            return 1;
}

int GetWordAndLetterCount(char* Line)
{
    int Word_Count = 0, Letter_Count = 0;
    for (int i = 0; i < 132; i++) {
        if ((Line[i] != ' ') && (Line[i] != 0) && (Line[i] != '\n'))
            Letter_Count++;
        else {
            if (Letter_Count % 2) {
                pthread_mutex_lock(&mutex);
                TotalOddWords++;
                pthread_mutex_unlock(&mutex);
                Word_Count++;
                Letter_Count = 0;
            } else {
                pthread_mutex_lock(&mutex);
                TotalEvenWords++;
                pthread_mutex_unlock(&mutex);
                Word_Count++;
                Letter_Count = 0;
            }
            if (Line[i] == 0)
                break;
        }
    }
    return Word_Count; // encode two return values
}

void* CountWords(void* arg)
{
    char inLine[132];

    while (true) {
        pthread_mutex_lock(&mutex);
        bool bDone = (GetNextLine(fd, inLine) == EOF);
        pthread_mutex_unlock(&mutex);

        if (bDone)
            break;

        int wordCount = GetWordAndLetterCount(inLine);

        pthread_mutex_lock(&mutex);
        TotalWords += wordCount;
        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main()
{
    fd = fopen("./InFile1.txt", "r"); // Open file for read

    struct timeval TimeStampStart, TimeStampStop;
    double ExeTime;
    gettimeofday(&TimeStampStart, NULL);

    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, CountWords, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&TimeStampStop, NULL);
    ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

    fclose(fd);
    pthread_mutex_destroy(&mutex);

    printf("Total Words = %8d\n", TotalWords);
    printf("Total Even Words = %7d\nTotal Odd Words = %7d\n", TotalEvenWords, TotalOddWords);
    printf("The time to read words was %f seconds\n", ExeTime);

    return 0;
}

