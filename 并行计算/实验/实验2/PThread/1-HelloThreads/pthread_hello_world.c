#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
const int NumThreads = 16;

static void* HelloFunc(void* pArg)
{ 
	printf("Hello Thread %d !\n", *((int*)pArg));
}

int main() 
{	
	int Num[NumThreads];
	pthread_t ThreadIDs[NumThreads];
    pthread_attr_t attr[NumThreads];

	for (int i = 0; i < NumThreads; i++) {
		Num[i] = i;
		pthread_attr_init(&attr[i]);
		pthread_attr_setdetachstate(&attr[i], PTHREAD_CREATE_JOINABLE);
	}

	for (int i = 0; i < NumThreads; i++) {
		int err = pthread_create(&ThreadIDs[i], &attr[i], HelloFunc, (void*)&Num[i]);
		if(err != 0) {
			printf("ERROR: pthread_create() return code: %d\n", err);
        }
	}

	for (int i = 0; i < NumThreads; i++) {
		pthread_join(ThreadIDs[0], NULL);
		printf("Thread %d end !\n", i);
	}

	return 0; 
}

