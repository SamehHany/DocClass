#include "train.h"

HashTable *skip;

void initSkip();

double train(const char *path, int N)
{
	int i, len;
	
	int path_strlen = strlen(path);
	int size = path_strlen + 100;
	char *cat_path = malloc(size);
	
	// Remove old training data
	strcpy(cat_path, "data/");
	char *cat_name_ptr = cat_path + strlen(cat_path);
	char **data_files = getItems(cat_path, &len);
	for (i = 0; i < len; i++) {
		strcpy(cat_name_ptr, data_files[i]);
		int status = remove(cat_path);
		if (status != 0)
			printf("Unable to delete \"%s\"\n", cat_name_ptr);
	}
	del_str_arr(data_files, len);
	
	char **dirs = getItems(path, &len);
	
	strcpy(cat_path, path);
	if (cat_path[path_strlen-1] != '/')
	{
		cat_path[path_strlen] = '/';
		cat_path[path_strlen+1] = '\0';
		path_strlen++;
	}
	cat_name_ptr = cat_path + path_strlen;
	
	initializeHash();

	initSkip();
	printf("training...\n");
	for (i = 0; i < len; i++)
	{
		if (size - path_strlen <= strlen(dirs[i]) )
		{
			free(cat_path);
			size = size*2;
			cat_path = malloc(size);
			strcpy(cat_path, path);
			cat_name_ptr = cat_path + path_strlen;
		}
		strcpy(cat_name_ptr, dirs[i]);
		trainCat(cat_path, N);
		printf("%s\tdone.\n", cat_path);
	}
	printf("\ntesting...\n");
	double prob = 0;
	long int total_weight = 0;
	for (i = 0; i < len; i++)
	{
		strcpy(cat_name_ptr, dirs[i]);
		long int weight;
		double cat_acc = accuracy(cat_path, dirs[i], &weight);
		prob += cat_acc * weight;
		total_weight += weight;
		printf("%s\taccuracy: %f%%\n", cat_path, cat_acc*100);
	}
	deleteHt(skip);
	free(cat_path);
	del_str_arr(dirs, len);
	return prob / total_weight;
}

int _Hn_int_cmp(const void *n1, const void *n2)
{
	return ( *( (int*)( *(HashNode**)n2 )->data ) ) - ( *( (int*)( *(HashNode**)n1 )->data ) );
	//int x1 = ( *( (int*)( *(HashNode**)n1 )->data ) );
	//int x2 = ( *( (int*)( *(HashNode**)n2 )->data ) );
	//return x2 - x1;
}

int doSkip(const char *str)
{
	int exists;
	HtGet(skip, (void*)str, strlen(str) + 1, 0, &exists);
	if (exists)
		return 1;
	return 0;
}

double trainCat(const char *path, int N)
{
	HashTable *ht = newHt();
	ht->cmp = &_str_cmp;
	
	int i, len;
	char **items = getItems(path, &len);
	
	int path_str_len = strlen( path );
	int fp_size = path_str_len + 100; // file path string size
	char *filepath = malloc(fp_size);
	strcpy(filepath, path);
	char *file_strptr = filepath + path_str_len; // pointer to file title
	
	int train_up_bound = len * TFRAC;
	for (i = 0; i < train_up_bound; i++)
	{
		if (fp_size - path_str_len <= strlen(items[i]) + 1)
		{
			free(filepath);
			fp_size = fp_size * 2;
			filepath = malloc(fp_size);
			strcpy(filepath, path);
			file_strptr = filepath + path_str_len;
		}
		file_strptr[0] = '/';
		strcpy(file_strptr+1, items[i]);
		char *file = readFile(filepath);
		int size;
		char **tokens = segment(file, &size);
		free( file );
	
		int j, found;
		/*for (j = 0; j < size; j++)
		{
			if (strcmp(tokens[j], "Lines") == 0)
				break;
		}
		for ( ; j < size; j++)*/
		for (j = 0; j < size; j++)
		{
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
			if ( doSkip(tokens[j]) )
				continue;
			int *count = (int*)HtInsert(ht, tokens[j], _strlen+1, 0, sizeof(*count), 0, &found);
			if (found)
				(*count)++;
			else *count = 1;
		}
		del_str_arr(tokens, size);
	}
	del_str_arr(items, len);
	
	HashNode **arr = Ht2arr(ht);
	//for (i = 0; i < ht->size; i++)
	//	printf("term: %s\tcount: %d\n", (char*)arr[i]->key, *( (int*)arr[i]->data ) );
	qsort(arr, ht->size, sizeof(*arr), &_Hn_int_cmp);
	/*if ( strcmp(path, "20_newsgroups/sci.crypt") == 0 ) // "comp.windows.x" and "sci.crypt"
	{
		for (i = 0; i < ht->size; i++)
		{
			printf("%d:\t%s -\t%d\n", i, (char*)arr[i]->key, *( (int*)arr[i]->data ) );
		}
	}*/
	
	/*while ( *((int*)arr[N-1]->data) == *((int*)arr[N]->data) )
	{
		N++;
	}*/
	
	double prob;
	int sum = 0;
	for (i = 0; i < ht->size; i++)
		sum += *( (int*)arr[i]->data );
	strcpy(filepath, "data/");
	
	const char *tmp_str;
	for (i = path_str_len; i > 0; i--)
	{
		if (path[i] == '/')
		{
			tmp_str = path+i+1;
			break;
		}
	}
	
	strcpy(filepath + strlen("data/"), tmp_str);
	FILE *outf = fopen(filepath, "w"); // outf: output file
	int no_terms;
	//if (N == -1)
	//	no_terms = (int)ht->size;
	//else no_terms = N < (int)ht->size? N: ht->size;
	no_terms = N < (int)ht->size? N: ht->size;
	fwrite( &no_terms, sizeof(no_terms), 1, outf );
	for (i = 0; i < no_terms; i++)
	{
		prob = (double)( *( (int*)arr[i]->data ) ) / sum;
		char _strlen = ( (char)arr[i]->key_size ) - 1;
		fwrite( &_strlen, sizeof(char), 1, outf );
		fwrite( (char*)arr[i]->key, sizeof(char), arr[i]->key_size-1, outf );
		fwrite( &prob, sizeof(prob), 1, outf );
	}
	fclose(outf);
	
	free(arr);
	free(filepath);
	deleteHt(ht);
}

double accuracy(const char *path, const char *cat_name, long int *weight)
{
	int i, len;
	char **items = getItems(path, &len);
	int path_strlen = strlen(path);
	char *filepath = malloc(path_strlen + 100); // file path
	strcpy(filepath, path);
	char *filename_ptr = filepath + path_strlen;
	int true_count = 0; // number of truely catecorized files
	
	int low_bound = len*TFRAC+1;
	*weight = len-low_bound+1;
	for (i = low_bound; i < len; i++)
	{
		filename_ptr[0] = '/';
		strcpy(filename_ptr+1, items[i]);
		char *cat = categorize(filepath);
		if(strcmp(cat, cat_name) == 0)
			true_count++;
		free(cat);
	}
	
	free(filepath);
	del_str_arr(items, len);
	
	return (double)true_count / (*weight);
}

long int _str_cmp(void *data1, void *data2, size_t size1, size_t size2)
{
	return strcmp((char*)data1, (char*)data2);
}

void del_str_arr(char **stra, size_t size)
{
	int i;
	for (i = 0; i < size; i++)
		free(stra[i]);
	free(stra);
}

void initSkip()
{
	skip = newHt();
	skip->cmp = &_str_cmp;
	HtInsert(skip, "a", 2, 0, 0, 0, 0);
	HtInsert(skip, "able", 5, 0, 0, 0, 0);
	HtInsert(skip, "about", 6, 0, 0, 0, 0);
	HtInsert(skip, "acros", 6, 0, 0, 0, 0);
	HtInsert(skip, "after", 6, 0, 0, 0, 0);
	HtInsert(skip, "all", 4, 0, 0, 0, 0);
	HtInsert(skip, "almost", 7, 0, 0, 0, 0);
	HtInsert(skip, "also", 5, 0, 0, 0, 0);
	HtInsert(skip, "am", 3, 0, 0, 0, 0);
	HtInsert(skip, "among", 6, 0, 0, 0, 0);
	HtInsert(skip, "an", 3, 0, 0, 0, 0);
	HtInsert(skip, "and", 4, 0, 0, 0, 0);
	HtInsert(skip, "any", 4, 0, 0, 0, 0);
	HtInsert(skip, "are", 4, 0, 0, 0, 0);
	//HtInsert(skip, "as", 3, 0, 0, 0, 0);
	HtInsert(skip, "at", 3, 0, 0, 0, 0);
	HtInsert(skip, "be", 3, 0, 0, 0, 0);
	HtInsert(skip, "because", 8, 0, 0, 0, 0);
	HtInsert(skip, "been", 5, 0, 0, 0, 0);
	HtInsert(skip, "but", 4, 0, 0, 0, 0);
	HtInsert(skip, "by", 3, 0, 0, 0, 0);
	HtInsert(skip, "can", 4, 0, 0, 0, 0);
	HtInsert(skip, "cannot", 7, 0, 0, 0, 0);
	HtInsert(skip, "could", 6, 0, 0, 0, 0);
	HtInsert(skip, "dear", 5, 0, 0, 0, 0);
	HtInsert(skip, "did", 4, 0, 0, 0, 0);
	HtInsert(skip, "do", 3, 0, 0, 0, 0);
	HtInsert(skip, "doe", 4, 0, 0, 0, 0);
	HtInsert(skip, "either", 7, 0, 0, 0, 0);
	HtInsert(skip, "else", 5, 0, 0, 0, 0);
	HtInsert(skip, "ever", 5, 0, 0, 0, 0);
	HtInsert(skip, "every", 6, 0, 0, 0, 0);
	HtInsert(skip, "for", 4, 0, 0, 0, 0);
	HtInsert(skip, "from", 5, 0, 0, 0, 0);
	HtInsert(skip, "get", 4, 0, 0, 0, 0);
	HtInsert(skip, "got", 4, 0, 0, 0, 0);
	HtInsert(skip, "had", 4, 0, 0, 0, 0);
	HtInsert(skip, "ha", 3, 0, 0, 0, 0);
	HtInsert(skip, "have", 5, 0, 0, 0, 0);
	HtInsert(skip, "he", 3, 0, 0, 0, 0);
	HtInsert(skip, "her", 4, 0, 0, 0, 0);
	//HtInsert(skip, "hers", 5, 0, 0, 0, 0);
	HtInsert(skip, "him", 4, 0, 0, 0, 0);
	HtInsert(skip, "hi", 3, 0, 0, 0, 0);
	HtInsert(skip, "how", 4, 0, 0, 0, 0);
	HtInsert(skip, "however", 8, 0, 0, 0, 0);
	HtInsert(skip, "i", 2, 0, 0, 0, 0);
	HtInsert(skip, "if", 3, 0, 0, 0, 0);
	HtInsert(skip, "in", 3, 0, 0, 0, 0);
	HtInsert(skip, "into", 5, 0, 0, 0, 0);
	HtInsert(skip, "i", 2, 0, 0, 0, 0);
	HtInsert(skip, "it", 3, 0, 0, 0, 0);
	//HtInsert(skip, "its", 4, 0, 0, 0, 0);
	HtInsert(skip, "just", 5, 0, 0, 0, 0);
	HtInsert(skip, "least", 6, 0, 0, 0, 0);
	HtInsert(skip, "let", 4, 0, 0, 0, 0);
	HtInsert(skip, "like", 5, 0, 0, 0, 0);
	HtInsert(skip, "likely", 7, 0, 0, 0, 0);
	HtInsert(skip, "may", 4, 0, 0, 0, 0);
	HtInsert(skip, "me", 3, 0, 0, 0, 0);
	HtInsert(skip, "might", 6, 0, 0, 0, 0);
	HtInsert(skip, "most", 5, 0, 0, 0, 0);
	HtInsert(skip, "must", 5, 0, 0, 0, 0);
	HtInsert(skip, "my", 3, 0, 0, 0, 0);
	HtInsert(skip, "neither", 8, 0, 0, 0, 0);
	HtInsert(skip, "no", 3, 0, 0, 0, 0);
	HtInsert(skip, "nor", 4, 0, 0, 0, 0);
	HtInsert(skip, "not", 4, 0, 0, 0, 0);
	HtInsert(skip, "of", 3, 0, 0, 0, 0);
	HtInsert(skip, "off", 4, 0, 0, 0, 0);
	HtInsert(skip, "often", 6, 0, 0, 0, 0);
	HtInsert(skip, "on", 3, 0, 0, 0, 0);
	HtInsert(skip, "only", 5, 0, 0, 0, 0);
	HtInsert(skip, "or", 3, 0, 0, 0, 0);
	HtInsert(skip, "other", 6, 0, 0, 0, 0);
	HtInsert(skip, "our", 4, 0, 0, 0, 0);
	HtInsert(skip, "own", 4, 0, 0, 0, 0);
	HtInsert(skip, "rather", 7, 0, 0, 0, 0);
	HtInsert(skip, "said", 5, 0, 0, 0, 0);
	HtInsert(skip, "say", 4, 0, 0, 0, 0);
	//HtInsert(skip, "says", 5, 0, 0, 0, 0);
	HtInsert(skip, "she", 4, 0, 0, 0, 0);
	HtInsert(skip, "should", 7, 0, 0, 0, 0);
	HtInsert(skip, "since", 6, 0, 0, 0, 0);
	HtInsert(skip, "so", 3, 0, 0, 0, 0);
	HtInsert(skip, "some", 5, 0, 0, 0, 0);
	HtInsert(skip, "than", 5, 0, 0, 0, 0);
	HtInsert(skip, "that", 5, 0, 0, 0, 0);
	HtInsert(skip, "the", 4, 0, 0, 0, 0);
	HtInsert(skip, "their", 6, 0, 0, 0, 0);
	HtInsert(skip, "them", 5, 0, 0, 0, 0);
	HtInsert(skip, "then", 5, 0, 0, 0, 0);
	HtInsert(skip, "there", 6, 0, 0, 0, 0);
	HtInsert(skip, "these", 6, 0, 0, 0, 0);
	HtInsert(skip, "they", 5, 0, 0, 0, 0);
	HtInsert(skip, "thi", 4, 0, 0, 0, 0);
	HtInsert(skip, "ti", 3, 0, 0, 0, 0);
	HtInsert(skip, "to", 3, 0, 0, 0, 0);
	HtInsert(skip, "too", 4, 0, 0, 0, 0);
	HtInsert(skip, "twa", 4, 0, 0, 0, 0);
	HtInsert(skip, "u", 2, 0, 0, 0, 0);
	HtInsert(skip, "want", 5, 0, 0, 0, 0);
	HtInsert(skip, "wa", 3, 0, 0, 0, 0);
	HtInsert(skip, "we", 3, 0, 0, 0, 0);
	HtInsert(skip, "were", 5, 0, 0, 0, 0);
	HtInsert(skip, "what", 5, 0, 0, 0, 0);
	HtInsert(skip, "when", 5, 0, 0, 0, 0);
	HtInsert(skip, "where", 6, 0, 0, 0, 0);
	HtInsert(skip, "which", 6, 0, 0, 0, 0);
	HtInsert(skip, "while", 6, 0, 0, 0, 0);
	HtInsert(skip, "who", 4, 0, 0, 0, 0);
	HtInsert(skip, "whom", 5, 0, 0, 0, 0);
	HtInsert(skip, "why", 4, 0, 0, 0, 0);
	HtInsert(skip, "will", 5, 0, 0, 0, 0);
	HtInsert(skip, "with", 5, 0, 0, 0, 0);
	HtInsert(skip, "would", 6, 0, 0, 0, 0);
	HtInsert(skip, "yet", 4, 0, 0, 0, 0);
	HtInsert(skip, "you", 4, 0, 0, 0, 0);
	HtInsert(skip, "your", 5, 0, 0, 0, 0);
	HtInsert(skip, "edu", 4, 0, 0, 0, 0);
	HtInsert(skip, "com", 4, 0, 0, 0, 0);
	HtInsert(skip, "net", 4, 0, 0, 0, 0);
	HtInsert(skip, "gov", 4, 0, 0, 0, 0);
	HtInsert(skip, "org", 4, 0, 0, 0, 0);
	HtInsert(skip, "eng", 4, 0, 0, 0, 0);
	HtInsert(skip, "cmu", 4, 0, 0, 0, 0);
	HtInsert(skip, "umd", 4, 0, 0, 0, 0);
	HtInsert(skip, "one", 4, 0, 0, 0, 0);
	HtInsert(skip, "two", 4, 0, 0, 0, 0);
	HtInsert(skip, "out", 4, 0, 0, 0, 0);
	HtInsert(skip, "now", 4, 0, 0, 0, 0);
	HtInsert(skip, "very", 5, 0, 0, 0, 0);
	HtInsert(skip, "much", 5, 0, 0, 0, 0);
	HtInsert(skip, "up", 3, 0, 0, 0, 0);
	HtInsert(skip, "ve", 3, 0, 0, 0, 0);
	HtInsert(skip, "re", 3, 0, 0, 0, 0);
	HtInsert(skip, "subject", 8, 0, 0, 0, 0);
	HtInsert(skip, "message", 8, 0, 0, 0, 0);
	HtInsert(skip, "date", 5, 0, 0, 0, 0);
	HtInsert(skip, "newsgroup", 10, 0, 0, 0, 0);
	HtInsert(skip, "id", 3, 0, 0, 0, 0);
	HtInsert(skip, "gmt", 4, 0, 0, 0, 0);
	HtInsert(skip, "cc", 3, 0, 0, 0, 0);
	HtInsert(skip, "sender", 7, 0, 0, 0, 0);
	HtInsert(skip, "an", 3, 0, 0, 0, 0);
	HtInsert(skip, "line", 5, 0, 0, 0, 0);
	HtInsert(skip, "university", 11, 0, 0, 0, 0);
	HtInsert(skip, "organization", 13, 0, 0, 0, 0);
	HtInsert(skip, "ca", 3, 0, 0, 0, 0);
	HtInsert(skip, "uk", 3, 0, 0, 0, 0);
	HtInsert(skip, "ac", 3, 0, 0, 0, 0);
	HtInsert(skip, "au", 3, 0, 0, 0, 0);
	HtInsert(skip, "de", 3, 0, 0, 0, 0);
	HtInsert(skip, "f", 2, 0, 0, 0, 0);
	HtInsert(skip, "co", 3, 0, 0, 0, 0);
	HtInsert(skip, "don", 4, 0, 0, 0, 0);
	HtInsert(skip, "more", 5, 0, 0, 0, 0);
	HtInsert(skip, "such", 5, 0, 0, 0, 0);
	HtInsert(skip, "many", 5, 0, 0, 0, 0);
	HtInsert(skip, "even", 5, 0, 0, 0, 0);
	//added
	HtInsert(skip, "s", 2, 0, 0, 0, 0);
	HtInsert(skip, "t", 2, 0, 0, 0, 0);
	HtInsert(skip, "c", 2, 0, 0, 0, 0);
	HtInsert(skip, "new", 4, 0, 0, 0, 0);
	HtInsert(skip, "apr", 4, 0, 0, 0, 0);
	HtInsert(skip, "srv", 4, 0, 0, 0, 0);
	HtInsert(skip, "cantaloupe", 11, 0, 0, 0, 0);
	HtInsert(skip, "article", 8, 0, 0, 0, 0);
	HtInsert(skip, "path", 5, 0, 0, 0, 0);
	HtInsert(skip, "reference", 10, 0, 0, 0, 0);
	HtInsert(skip, "alt", 4, 0, 0, 0, 0);
	HtInsert(skip, "was", 4, 0, 0, 0, 0);
	HtInsert(skip, "-", 2, 0, 0, 0, 0);
	//HtInsert(skip, "state", 6, 0, 0, 0, 0);
}
