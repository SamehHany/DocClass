#ifndef SEGMENTER_H
#define SEGMENTER_H

#include "common.h"

int isAlphaNum(const char *ch, int index);

void extractFromStr(char **target, const char *source, int start, int end);

char **segment(const char *text, int *size);

#endif
