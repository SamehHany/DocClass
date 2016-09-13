#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdlib.h>
#include <math.h>
#include "hashfunction.h"
#include "prime.h"

typedef struct HashNode {
	void *key;
	void *data;
	size_t key_size;
	size_t data_size;
	struct HashNode *next;
} HashNode;

HashNode *newHn();

void deleteHn(HashNode *_node);

void deleteHnAfter(HashNode *_node);

typedef struct {
	size_t table_size;
	size_t size; // size: size of data stored
	void *data; // Hash function data. e.g: constants, random variables, prime numbers, ...
	HashNode **table;
	unsigned long int (*hash)(unsigned long int, size_t, void*);
	unsigned long int (*prehash)(void*, size_t);
	long int (*cmp)(void*, void*, size_t, size_t);
} HashTable;

HashTable *newHt();

HashTable *newHtOfSize(long int size, int prime);

//HashTable *newHtOfPrimeSize(long int size);

void deleteHt(HashTable *T);

void* HtInsert(HashTable *Ht, void *key, size_t key_size, void *data, size_t data_size, int prime, int *found);

void expandHt(HashTable *Ht, int prime);

void* HtGet(HashTable *Ht, void *key, size_t key_size, size_t *data_size, int *exists);

HashNode** Ht2arr(HashTable *Ht); // Convert a Hash Table to a Sorted Array

void copy(void *dest, void *source, size_t size); // Size in bytes

long int _cmp(void *data1, void *data2, size_t size1, size_t size2);

#endif
