CC=gcc
CFLAGS= -lm -g
#=========================================================

DEPS = categorize.h common.h  \
	fileio.h segment.h train.h \
	hash/hashfunction.h hash/hashtable.h hash/prime.h

OBJ = main.o categorize.o \
	fileio.o segment.o train.o \
	hash/hashfunction.o  hash/hashtable.o  hash/prime.o

#=========================================================
ProgName = 'categorize'
#=========================================================

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all : $(ProgName)

$(ProgName) : $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o hash/*.o $(ProgName)
