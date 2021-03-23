CC = gcc
CFLAGS = -g -Wall

TARGET = waffsh

all: clean $(TARGET)

$(TARGET): main.o read.o parser.o executor.o malloc.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o read.o parser.o executor.o malloc.o

main.o: main.c read.h constants.h
	$(CC) $(CFLAGS) -c main.c

read.o: read.c read.h constants.h
	$(CC) $(CFLAGS) -c read.c

parser.o: parser.c parser.h constants.h executor.h utils/malloc.h
	$(CC) $(CFLAGS) -c parser.c

executor.o: executor.c executor.h
	$(CC) $(CFLAGS) -c executor.c

malloc.o: utils/malloc.c utils/malloc.h
	$(CC) $(CFLAGS) -c utils/malloc.c

clean:
	rm -f $(TARGET) *.o