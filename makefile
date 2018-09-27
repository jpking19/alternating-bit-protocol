CC = gcc
CFLAGS = -g -Wall

all: abp

abp: student2.o project2.o
	$(CC) $(CFLAGS) student2.c project2.c -o p2

student2.o: student2.c project2.h
	$(CC) $(CFLAGS) -c student2.c

project2.o: project2.c project2.h
	$(CC) $(CFLAGS) -c project2.c


clean:
	rm -rf p2 project2.o student2.o
