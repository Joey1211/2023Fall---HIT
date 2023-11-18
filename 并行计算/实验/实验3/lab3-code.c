#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char *argv[]);
int prime_part(int id, int p, int n);

int main(int argc, char *argv[]){
    int id;
    int n;
    int p;
    int total;
    int total_part;
    double start_time, end_time;

    MPI_Init(&argc, &argv);               // ��ʼ��MPI����
    MPI_Comm_rank(MPI_COMM_WORLD, &id);   // ��ȡ��ǰ���̵�ID
    MPI_Comm_size(MPI_COMM_WORLD, &p);    // ��ȡ�ܽ�����

    if (argc < 2){
        if (id == 0){
            printf("Usage: %s <N>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    n = atoi(argv[1]);
    total = 0;

    start_time = MPI_Wtime(); // ��¼��ʼʱ��

    total_part = prime_part(id, p, n);
    MPI_Reduce(&total_part, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime(); // ��¼����ʱ��

    if (id == 0){
        printf("\n");
        printf("Between 2 and %d, there are %d primes\n", n, total);
        printf("Execution time: %f seconds\n", end_time - start_time);
    }

    MPI_Finalize();

    return 0;
}

int prime_part(int id, int p, int n){
    int i;
    int j;
    int prime;
    int total_part;

    total_part = 0;
    int temp=n/p;

    for (i = 2 + id*temp; i <= (id+1)*temp; i ++){
        prime = 1;

        for (j = 2; j * j <= i; j++){
            if (i % j == 0){
                prime = 0;
                break;
            }
        }
        if (prime){
            total_part++;
        }
    }

    return total_part;
}

