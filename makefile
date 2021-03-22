CC = gcc
CFLAGS = -g -Wall

TARGET = waffsh

all: clean $(TARGET)

$(TARGET): main.o read.o parser.o executor.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o read.o parser.o executor.o

main.o: main.c read.h constants.h
	$(CC) $(CFLAGS) -c main.c

read.o: read.c read.h
	$(CC) $(CFLAGS) -c read.c

parser.o: parser.c parser.h constants.h executor.h
	$(CC) $(CFLAGS) -c parser.c

executor.o: executor.c executor.h
	$(CC) $(CFLAGS) -c executor.c

clean:
	rm -f $(TARGET) *.o