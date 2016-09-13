#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*int power(int num, int e)
{
	if (e == 0)
		return 1;
	int i;
	int r = 1; // r: return value
	for (i = 0; i < e; i++)
		r *= num;
	return r;
}

char *int2str(int num)
{
	char *str;
	int size = 1;
	int temp = num;
	while( (temp/=10) > 0 )
		size++;
	str = (char*)malloc(size + 1);
	int i;
	int e = size-1;
	for (i = 0; i < size; i++) {
		temp = num / power(10, e);
		str[i] = (char) (num - temp*power(10, e) + 48);
		printf("%c", str[i]);
		e--;
	}
	printf("\n");
	str[size] = '\0';
	return str;
}*/

char *int2str(int num)
{
	char *str;
	int size = 1;
	int temp = num;
	while( (temp/=10) > 0 )
		size++;
	str = (char*)malloc(size + 1);
	
	sprintf(str, "%d", num);
	str[size] = '\0';
	return str;
}

int str2int(const char *str)
{
	int r = 0; // return value
	
	int len = 0;
	while(str[len] != '\0')
		len++;
	
	int i = 0;
	for (i = 0; i < len; i++)
		r += (int) ( str[i] - '0' ) * (len - i);
	return r;
}

int main(int argc, char **argv)
{
	if (argc != 4 && argc != 5) {
		printf("Number of agrguments: %d\n", argc);
		printf("Invalid number of arguments\n");
	}
	else
	{
		FILE *in = fopen(argv[2], "r");
		FILE *out = fopen(argv[3], "w");
		int i;
		int size;
		char *str;
		if (out == 0) {
			printf("Could not open file \"%s\"\n", argv[3]);
			return 0;
		}
		if (in == 0) {
			printf("Could not open file \"%s\"\n", argv[2]);
			return 0;
		}
		else
		{
			fseek(in, 0, SEEK_END); // seek to end of file
			size = ftell(in) + 1;
			fseek(in, 0, SEEK_SET);
			
			int ch;
			str = (char*) malloc(size);
			i = 0;
			while  ( ( ch = fgetc( in ) ) != EOF ) {
				str[i] = (char)ch;
				i++;
			}
			str[size-1] = '\0';
			fclose(in);
			//printf("%s\n", str);
		}
		
		int start = 0;
		int tabs;
		if (argc == 4)
			tabs = 0;
		else tabs = str2int(argv[4]);
		int j;
		int len = 0;
		char *num;
		while(argv[1][len] != '\0')
			len++;
		int index = 0;
		for(i = 0; i < size; i++) {
			//printf("no Floating point exception at the begining of the loop");
			if (str[i] == ',' || str[i] == '\0' || str[i] == '\n') {
				for (j = 0; j < tabs; j++) {
					fputc( (int)'\t', out );
				}
				fwrite("HtInsert(", sizeof(char), strlen("HtInsert("), out);
				for(j = 0; j < len; j++) {
					fputc( (int)argv[1][j], out );
				}
				fwrite(", \"", sizeof(char), 3, out);
				fwrite(str + start, sizeof(str[0]), i - start, out);
				fwrite("\", ", sizeof(char), 3, out);
				char *num = int2str(i - start + 1);
				fwrite(num, sizeof(*num), strlen(num), out);
				free(num);
				fwrite(", 0, 0, 0, 0);\n", sizeof(char), strlen(", 0, 0, 0, 0);\n"), out);
				
				start = i+1;
			}
		}
		fclose(out);
		free(str);
	}

	return 0;
}
