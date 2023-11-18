#include <stdio.h>
#include <omp.h>

int main() {
	int i;
#pragma omp parallel
{
	printf("Hello World\n");
	for(i=0; i<4; i++) {
		printf("Iter:%d\n",i);
	}

	printf("GoodBye World\n");
}

	return 0;
}