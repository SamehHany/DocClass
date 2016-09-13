#include "prime.h"

int isPrime(long int num)
{
	if (num % 2 == 0)
		return 0;
	long int limit = (long int)sqrt( (double)num ) + 1;
	long int i;
	for ( i = 3; i < limit; i+=2 )
	{
		if (num % i == 0)
			return 0;
	}
	return 1;
}

long int generatePrimeGreaterThan(long int num)
{
	if (num % 2 == 0)
		num++;
	while ( !isPrime( num ) ) { num+=2; }
	return num;
}

long int primeAround(long int n)
{
	long int up, down;
	if (n % 2 == 0) {
		up = n + 1;
		down = n - 1;
	}
	else {
		up = n;
		down = n - 2;
	}
	while (1)
	{
		if ( isPrime(up) )
			return up;
		if ( isPrime(down) )
			return down;
		up += 2;
		down -= 2;
	}
}
