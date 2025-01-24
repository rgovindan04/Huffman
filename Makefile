CC=clang
CFLAGS=-Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wstrict-prototypes -pedantic
LDFLAGS =-lm

HUFF_EXEC = huff
DEHUFF_EXEC = dehuff
BRTEST = brtest
BWTEST = bwtest
NODETEST = nodetest
PQTEST = pqtest

OBJS=\
     bitreader.o \
     bitwriter.o \
     node.o \
     pq.o \

all: $(HUFF_EXEC) $(DEHUFF_EXEC) $(BRTEST) $(BWTEST) $(NODETEST) $(PQTEST)

$(HUFF_EXEC): huff.o $(OBJS)
	$(CC) $(LDFLAGS) -o $(HUFF_EXEC) huff.o $(OBJS)

$(DEHUFF_EXEC): dehuff.o $(OBJS)
	$(CC) $(LDFLAGS) -o $(DEHUFF_EXEC) dehuff.o $(OBJS)

$(BRTEST): brtest.o $(OBJS)
	$(CC) $(LDFLAGS) -o $(BRTEST) brtest.o $(OBJS)

$(BWTEST): bwtest.o $(OBJS)
	$(CC) $(LDFLAGS) -o $(BWTEST) bwtest.o $(OBJS)

$(NODETEST): nodetest.o $(OBJS)
	$(CC) $(LDFLAGS) -o $(NODETEST) nodetest.o $(OBJS)

$(PQTEST): pqtest.o $(OBJS)
	$(CC) $(LDFLAGS) -o $(PQTEST) pqtest.o $(OBJS)

bitreader.o: bitreader.c
	$(CC) $(CFLAGS) -c bitreader.c

bitwriter.o: bitwriter.c
	$(CC) $(CFLAGS) -c bitwriter.c

node.o: node.c
	$(CC) $(CFLAGS) -c node.c

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c


%.o : %.c
	$(CC) $(CFLAGS) -c $<


clean:
	rm -f $(HUFF_EXEC) $(DEHUFF_EXEC) $(BRTEST) $(BWTEST) $(NODETEST) $(PQTEST) $(wildcard *.o)

format:
	clang-format -i --style=file *.[ch]


