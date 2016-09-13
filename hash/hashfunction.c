#include "hashfunction.h"

int _hash_initialized = 0;

/*unsigned long int _prehash(void *data, size_t size)
{
	int ws = sizeof(long int); // size of word
	if (size == ws)
		return *( (unsigned long int*)data );
	unsigned long int i, ret = 0;
	unsigned char *dataptr = (unsigned char*)data; // byte pointer to data
	unsigned char *retptr = (unsigned char*)(&ret);
	for (i = 0; i < size; i++)
		retptr[i % ws] += (dataptr[i] ^ _randbyte) + (i % 3 ? size: -size);
	return ret ^ _randword;
}*/

unsigned long int _prehash(void *data, size_t size)
{
	register long long int len;
	register unsigned char *p;
	register long x;

	len = size;
	p = (unsigned char *) data;
	x = *p << 7;
	while (--len >= 0)
		x = (1000003*x) ^ *p++;
	x ^= size;

	return x;
}

unsigned long int divhash(unsigned long int num, unsigned long int m)
{
	return num % m;
}

void initializeHash()
{
	if (!_hash_initialized)
	{
		FILE *urandom = fopen ("/dev/urandom", "r");
		if (urandom == NULL) {
			fprintf (stderr, "Cannot open /dev/urandom!\n");
			exit (1);
		}
		fread (&_randword, sizeof (_randword), 1, urandom);
		fread (&_randbyte, sizeof (_randbyte), 1, urandom);
		fclose(urandom);
		//_randword = 179770580453629865;
		//_randbyte = 10;
		_hash_initialized = 1;
	}
}
