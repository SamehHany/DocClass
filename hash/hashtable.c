#include "hashtable.h"

HashNode *newHn()
{
	HashNode *n = malloc( sizeof(HashNode) );
	n->key = 0;
	n->data = 0;
	n->next = 0;
	return n;
}

void deleteHn(HashNode *_node)
{
	if (_node != 0)
	{
		free(_node->key);
		free(_node->data);
		deleteHn( _node->next );
		free(_node);
	}
}

void deleteHnAfter(HashNode *_node)
{
	if (_node != 0 && _node->next != 0)
	{
		HashNode *tmp = _node->next;
		_node->next = tmp->next;
		deleteHn(tmp);
	}
}

unsigned long int _hash(unsigned long int num, unsigned long int m, void *data)
{
	return divhash(num, m);
}

HashTable *newHt()
{
	return newHtOfSize(101, 0);
}

HashTable *newHtOfSize(long int size, int prime)
{
	HashTable *t = malloc( sizeof(HashTable) );
	if ( prime )
		size = generatePrimeGreaterThan(size);
	t->table_size = size;
	t->size = 0;
	t->data = 0;
	t->table = malloc ( size * sizeof(HashNode*) );
	long int i;
	for (i = 0; i < size; i++)
		t->table[i] = 0;
	t->hash = &_hash;
	t->prehash = &_prehash;
	t->cmp = &_cmp;
	return t;
}

/*HashTable *newHtOfPrimeSize(long int size)
{
	size = generatePrimeGreaterThan(size);
	return newHtOfSize(size);
}*/

void deleteHt(HashTable *T)
{
	if (T != 0)
	{
		free( T->data );
		long int i;
		for (i = 0; i < T->table_size; i++)
			deleteHn( T->table[i] );
		free( T->table );
		free( T );
	}
}

void* HtInsert(HashTable *Ht, void *key, size_t key_size, void *data, size_t data_size, int prime, int *found)
{
	unsigned long int index = (*Ht->hash)( (*Ht->prehash)(key, key_size), Ht->table_size, Ht->data );
	if (found != 0)
		*found = 0;
	HashNode *tmp = Ht->table[index];
	while (tmp != 0)
	{
		if ( (*Ht->cmp)(key, tmp->key, key_size, tmp->key_size) == 0)
		{
			if (found != 0)
				*found = 1;
			return tmp->data;
		}
		tmp = tmp->next;
	}
	Ht->size++;
	if (Ht->size > Ht->table_size)
	{
		expandHt(Ht, prime);
		index = (*Ht->hash)( (*Ht->prehash)(key, key_size), Ht->table_size, Ht->data );
	}
	HashNode *n = newHn();
	n->key = malloc(key_size);
	copy( n->key, key, key_size);
	n->key_size = key_size;
	if (data_size != 0)
		n->data = malloc(data_size);
	if (data != 0)
		copy( n->data, data, data_size);
	n->data_size = data_size;
	n->next = Ht->table[index];
	Ht->table[index] = n;
	return n->data;
}

void expandHt(HashTable *Ht, int prime)
{
	long int oldSize = Ht->table_size;
	Ht->table_size = Ht->table_size * 2;
	if ( prime )
		Ht->table_size = generatePrimeGreaterThan( Ht->table_size );
	HashNode **tmp_t = Ht->table; // tmp_t: Temporary Table
	Ht->table = malloc( Ht->table_size * sizeof(HashNode*) );
	long int i;
	for ( i = 0; i < Ht->table_size; i++)
		Ht->table[i] = 0;
	HashNode *tmp, *tmpnxt;
	unsigned long int index;
	for ( i = 0; i < oldSize; i++)
	{
		tmp = tmp_t[i];
		while (tmp != 0)
		{
			index = (*Ht->hash)( (*Ht->prehash)(tmp->key, tmp->key_size), Ht->table_size, Ht->data );
			tmpnxt = tmp->next;
			tmp->next = Ht->table[index];
			Ht->table[index] = tmp;
			tmp = tmpnxt;
		}
	}
	free( tmp_t );
}

void* HtGet(HashTable *Ht, void *key, size_t key_size, size_t *data_size, int *exists)
{
	if (exists != 0)
		*exists = 0;
	long int index = (*Ht->hash)( (*Ht->prehash)(key, key_size), Ht->table_size, Ht->data );
	HashNode *n = Ht->table[index];
	while ( n != 0 )
	{
		if ( !(*Ht->cmp)(key, n->key, key_size, n->key_size) )
			break;
		n = n->next;
	}
	if (n != 0)
	{
		if (exists != 0)
			*exists = 1;
		if (data_size != 0)
			*data_size = n->data_size;
		return n->data;
	}
	return 0;
}

HashNode** Ht2arr(HashTable *Ht)
{
	HashNode **arr = malloc( Ht->size * sizeof(*arr) );
	long int i, index = 0;
	for(i = 0; i < Ht->table_size; i++)
	{
		HashNode *tmp = Ht->table[i];
		while (tmp != 0)
		{
			arr[index] = tmp;
			index++;
			tmp = tmp->next;
		}
	}
	return arr;
}

void copy(void *dest, void *source, size_t size)
{
	long int i;
	for ( i = 0; i < size; i++)
		( (char*)dest )[i] = (char) ( (const char*)source )[i];
}

long int _cmp(void *data1, void *data2, size_t size1, size_t size2)
{
	if (size1 != size2)
		return size1 - size2;
	long int m = size1 < size2 ? size1 : size2; // m: minimum
	long int i;
	for (i = 0; i < m; i++)
	{
		if ( ((char*)data1)[i] != ((char*)data2)[i] )
			return ((char*)data1)[i] - ((char*)data2)[i];
	}
	return 0;
}
