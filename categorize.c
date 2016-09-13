#include "categorize.h"

void readTerm(FILE *fp, char *text, double *prob);

int _min(double *arr, int len);

void lowercase(char *str)
{
	int i = 0;
	while (str[i] != '\0')
	{
		if ( (str[i] >= 65 && str[i] <= 90) )
		{
			str[i] = str[i] + 32;
		}
		i++;
	}
}

char *categorize(const char *fp) // fp: file path
{
	char *filetext = readFile(fp);
	int no_tok; // number of tokens
	char **tokens = segment(filetext, &no_tok);
	free(filetext);
	int i, j, len, dd_size = 100; // data directory size
	char *datadp = malloc(dd_size); // datadp: data directory path
	strcpy(datadp, "data/");
	char **datafiles = getItems(datadp, &len);
	double *probs = malloc(len * sizeof(*probs));
	for (i = 0; i < len; i++)
		probs[i] = 0;
	int datadp_strlen = strlen(datadp);
	char *dfpp = datadp + datadp_strlen; // data file path pointer
	initializeHash();
	for (i = 0; i < len; i++)
	{
		if (strlen(datafiles[i]) + datadp_strlen >= dd_size)
		{
			free(datadp);
			dd_size *= 2;
			datadp = malloc(dd_size);
			strcpy(datadp, "data/");
			dfpp = datadp + datadp_strlen;
		}
		strcpy(dfpp, datafiles[i]);
		FILE *f = fopen(datadp, "r");
		int no_terms;
		fread(&no_terms, sizeof(no_terms), 1, f);
		char text[256];
		double prob;
		HashTable *ht = newHt();
		for (j = 0; j < no_terms; j++)
		{
			readTerm(f, text, &prob);
			HtInsert(ht, text, strlen(text) + 1, &prob, sizeof(prob), 0, 0);
		}
		fclose(f);
		
		for (j = 0; j < no_tok; j++)
		{
			int exists;
			lowercase(tokens[j]);
			int _strlen = strlen(tokens[j]);
			if (_strlen > 3)
			{
				if (tokens[j][_strlen-3] == 'i' && tokens[j][_strlen-2] == 'e' && tokens[j][_strlen-1] == 's')
				{
					tokens[j][_strlen-3] = 'y';
					tokens[j][_strlen-2] = '\0';
				}
			}
			if (tokens[j][_strlen-1] == 's' && _strlen > 1)
				tokens[j][_strlen-1] = '\0';
			
			double *tmp = (double*)HtGet(ht, tokens[j], strlen(tokens[j]) + 1, 0, &exists);
			if (exists)
				probs[i] +=  log(*tmp);
		}
	
		deleteHt(ht);
	}
	free(datadp);
	for (i = 0; i < no_tok; i++)
		free(tokens[i]);
	free(tokens);
	int m = _min(probs, len);
	char *ret = malloc( strlen(datafiles[m]) + 1 );
	strcpy(ret, datafiles[m]);
	free(probs);
	for (i = 0; i < len; i++)
		free(datafiles[i]);
	free(datafiles);
	return ret;
}

void readTerm(FILE *fp, char *text, double *prob)
{
	char len;
	fread(&len, sizeof(len), 1, fp);
	fread(text, sizeof(*text), len, fp);
	text[len] = '\0';
	fread(prob, sizeof(prob), 1, fp);
}

int _min(double *arr, int len)
{
	int i, m = 0; // m: index of maximum
	for (i = 1; i < len; i++)
	{
		if (arr[i] < arr[m])
			m = i;
	}
	return m;
}
