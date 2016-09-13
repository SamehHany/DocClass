#include "fileio.h"

char *readFile(const char *path)
{
	FILE *f = fopen(path, "r");
	int size;
	char *str;
	
	if (f == 0) {
		printf("Could not open file \"%s\"\n", path);
		return 0;
	}
	else
	{
		fseek(f, 0, SEEK_END); // seek to end of file
		size = ftell(f);
		fseek(f, 0, SEEK_SET);
			
		int ch;
		str = (char*) malloc(size+1);
		int i = 0;
		while  ( ( ch = fgetc( f ) ) != EOF ) {
			str[i] = (char)ch;
			i++;
		}
		str[size] = '\0';
		fclose(f);
		//printf("No problem during reading file\n");
		return str;
	}
}

char** getItems(const char *path, int *len)
{
	DIR *d;
	struct dirent *dir;
	int size = 100;
	char **items = malloc( size * sizeof(char*) );
	*len = 0;
	d = opendir(path);
	//dir = readdir(d); dir = readdir(d); // get rid of "." and ".."
	if (d)
	{
		while ( (dir = readdir(d)) != NULL )
		{
			if (dir->d_name[0] == '.')
			{
				if (strlen(dir->d_name) == 1)
					continue;
				else if (strlen(dir->d_name) == 2) {
					if (dir->d_name[1] == '.')
						continue;
				}
			}
			if (*len == size)
			{
				char **tmp = items;
				size = size * 2;
				items = malloc( size * sizeof(char*) );
				int i;
				for (i = 0; i < *len; i++)
					items[i] = tmp[i];
				free(tmp);
			}
			items[*len] = malloc( strlen(dir->d_name) + 1 );
			strcpy(items[*len], dir->d_name);
			(*len)++;
			//printf("%s\n", dir->d_name);
		}

		closedir(d);
	}
	return items;
}
