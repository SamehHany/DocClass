#include "common.h"
#include "fileio.h"
#include "segment.h"
#include "train.h"
#include "categorize.h"

int main(int argc, char **argv)
{
	if (argv[1][0] == '-' && argv[1][1] == 't')
	{
		double acc = train(argv[2], 200);
		printf("\nTotal accuracy: %f%%\n", acc*100);
	}
	else if (argv[1][0] == '-' && argv[1][1] == 'c')
	{
		char *cat = categorize(argv[2]);
		printf("category: %s\n", cat);
		free(cat);
	}
	
	return 0;
}
