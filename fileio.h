#ifndef FILEIO_H
#define FILEIO_H


#include "common.h"
#include <dirent.h>

char *readFile(const char *path);

char** getItems(const char *path, int *len);

#endif
