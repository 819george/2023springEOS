CC=gcc
CFLAGS=-Wall -g

all: lab5

lab5: lab5.o sockop.o
	$(CC) $(CFLAGS) -o lab5 lab5.o sockop.o

lab5.o: lab5.c sockop.h
	$(CC) $(CFLAGS) -c lab5.c

sockop.o: sockop.c sockop.h
	$(CC) $(CFLAGS) -c sockop.c

clean:
	rm -f lab5 *.o
