#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main ( int argc, char *argv[] );
int prime_part ( int id, int p, int n );

int main ( int argc, char *argv[] )

	/******************************************************************************/
	/*
	   Purpose:

	   PRIME counts the prime numbers between 2 and N.

	   Discussion:

	   We divide the calculation up into P parts.

	   Part 0   checks 2+0,   2+P+0,   2+2*P+0, ...
	   Part 2   checks 2+1,   2+P+1,   2+2*P+1, ...
	   Part P-1 checks 2+P-1, 2+2*P-1, 2+3*P-1, ...

	   Right now, the outer loop executes all P parts.
	   But it would be simple to make an MPI version, in which the
	   program with identifier ID only executes part ID.

	   Modify this program so that it runs under MPI.

*/
{
	int id;
	int n = 100000;
	int p;
	int total;
	int total_part;

	p = 4;

	total = 0;

	for ( id = 0; id < p; id++ )
	{
		total_part = prime_part ( id, p, n );
		total = total + total_part;
	}
	printf ( "\n" );
	printf ( "     Between 2 and %d, there are %d primes\n", n, total );

	return 0;
}
int prime_part ( int id, int p, int n )
{
	int i;
	int j;
	int prime;
	int total_part;

	total_part = 0;

	for ( i = id + 2; i <= n; i += p )
	{
		prime = 1;

		for ( j = 2; j < i; j++ )
		{
			if ( i % j == 0 )
			{
				prime = 0;
				break;
			}
		}
		if ( prime )
		{
			total_part = total_part + 1;
		}
	}

	return total_part;
}
