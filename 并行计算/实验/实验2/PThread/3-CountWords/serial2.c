#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

FILE *fd;
int TotalEvenWords = 0, TotalOddWords = 0, TotalWords = 0;
pthread_mutex_t mutex; // 互斥量

int GetNextLine(FILE *f, char *Line)
{
	if (fgets(Line, 132, f) == NULL)
		if (feof(f))
			return EOF;
		else
			return 1;
}

int GetWordAndLetterCount(char *Line)
{
	int Word_Count = 0, Letter_Count = 0;
	for (int i = 0; i < 132; i++)
	{
		if ((Line[i] != ' ') && (Line[i] != 0) && (Line[i] != '\n'))
			Letter_Count++;
		else {
			if (Letter_Count % 2) {
				pthread_mutex_lock(&mutex); // 加锁
				TotalOddWords++;
				Word_Count++;
				Letter_Count = 0;
				pthread_mutex_unlock(&mutex); // 解锁
			}
			else {
				pthread_mutex_lock(&mutex); // 加锁
				TotalEvenWords++;
				Word_Count++;
				Letter_Count = 0;
				pthread_mutex_unlock(&mutex); // 解锁
			}
			if (Line[i] == 0)
				break;
		}
	}
	return (Word_Count);
}

void *CountWords(void *arg)
{
	char inLine[132];
	while (true)
	{
		pthread_mutex_lock(&mutex); // 加锁
		if (GetNextLine(fd, inLine) == EOF)
		{
			pthread_mutex_unlock(&mutex); // 解锁
			break;
		}
		pthread_mutex_unlock(&mutex); // 解锁

		int wordCount = GetWordAndLetterCount(inLine);

		pthread_mutex_lock(&mutex); // 加锁
		TotalWords += wordCount;
		pthread_mutex_unlock(&mutex); // 解锁
	}

	pthread_exit(NULL);
}

int main()
{
	fd = fopen("./InFile1.txt", "r");

	struct timeval TimeStampStart, TimeStampStop;
	double ExeTime;
	gettimeofday(&TimeStampStart, NULL);

	pthread_mutex_init(&mutex, NULL); // 初始化互斥量

	pthread_t thread1, thread2, thread3; // 创建三个线程
	pthread_create(&thread1, NULL, CountWords, NULL);
	pthread_create(&thread2, NULL, CountWords, NULL);
	pthread_create(&thread3, NULL, CountWords, NULL);


	pthread_join(thread1, NULL); // 等待线程结束
	pthread_join(thread2, NULL);
	pthread_join(thread3, NULL);


	pthread_mutex_destroy(&mutex); // 销毁互斥量

	gettimeofday(&TimeStampStop, NULL);
	ExeTime = (double)(TimeStampStop.tv_sec - TimeStampStart.tv_sec) + (double)(TimeStampStop.tv_usec - TimeStampStart.tv_usec) * 1e-6;

	fclose(fd);

	printf("Total Words = %8d\n", TotalWords);
	printf("Total Even Words = %7d\nTotal Odd Words = %7d\n", TotalEvenWords, TotalOddWords);
	printf("The time to read words was %f seconds\n", (ExeTime));

	return 0;
}

