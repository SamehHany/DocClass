#ifndef TRAIN_H
#define TRAIN_H

#include "common.h"
#include "fileio.h"
#include "segment.h"
#include "hash/hashtable.h"
#include "categorize.h"

#define TFRAC 0.75	// Train Fraction

double train(const char *path, int N);

double trainCat(const char *path, int N);

long int _str_cmp(void *data1, void *data2, size_t size1, size_t size2);

void del_str_arr(char **stra, size_t size);

double accuracy(const char *path, const char *cat_name, long int *weight);

#endif
