#include "segment.h"

int isAlphaNum(const char *ch, int index)
{
	register char byte = ch[index];
	if (byte < 0 && index > 0) {
		register char prev_byte = ch[index-1];
		if (prev_byte == 0xdb) {
			if (byte >= 0xa1 && byte <= 0xba)
				return 1;
		}
		else if (prev_byte == 0xd9) {
			if ( (byte >= 0x80 && byte <= 0x8a) || (byte >= 0xa0 && byte <= 0xa9) )
				return 1;
		}
		else if (prev_byte == 0xdb) {
			if (byte >= 0xb0 && byte <= 0xb9)
				return 1;
		}
	}
	else if ( (byte >= 'a' && byte <= 'z') ||  (byte >= 'A' && byte <= 'Z') || (byte >= '0' && byte <= '9') || byte == '-' ) {
		return 1;
	}
	else return 0;
}

void extractFromStr(char **target, const char *source, int start, int end)
{
	int size = end - start;
	*target = (char*) malloc(size + 1);
	
	int i;
	int index = 0;
	for (i = start; i < end; i++) {
		(*target)[index] = source[i];
		index++;
	}
	(*target)[size] = '\0';
}

char **segment(const char *text, int *size)
{
	*size = 0;
	int skip = 1; // skip = 1 when indexing white space or characters
		      // to be skipped, otherwise skip = 0
	
	int i = 0;
	while (text[i] != '\0')
	{
		if ( isAlphaNum(text, i) )
		{
			if(skip == 1)
				(*size)++;
			
			skip = 0;
		}
		else {
			skip = 1;
		}
		i++;
	}
	
	char ** tokens = (char**) malloc( (*size) * sizeof(char*) );
	
	int start = 0;
	int index = 0;
	skip = 1;
	i = 0;
	while (1)
	{
		while (text[i] >= 0xd8 && text[i] <= 0xdb)
			i++;
		if ( isAlphaNum(text, i) )
		{
			if (skip == 1) {
				start = i;
				
			}
			skip = 0;
		}
		else {
			if (skip == 0) {
				extractFromStr(&tokens[index], text, start, i);
				index++;
			}
			skip = 1;
		}
		if (text[i] == '\0')
			break;
		
		i++;
	}
	//printf("No problem while parsing\n");
	return tokens;
}
