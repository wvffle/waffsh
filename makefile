CC = gcc
CFLAGS = -g -Wall

TARGET = waffsh

all: clean $(TARGET)

$(TARGET): main.o read.o parser.o executor.o utils.o history.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o read.o parser.o executor.o utils.o history.o

main.o: main.c read.h constants.h
	$(CC) $(CFLAGS) -c main.c

read.o: read.c read.h constants.h
	$(CC) $(CFLAGS) -c read.c

parser.o: parser.c parser.h constants.h executor.h utils.h
	$(CC) $(CFLAGS) -c parser.c

executor.o: executor.c executor.h utils.h
	$(CC) $(CFLAGS) -c executor.c

history.o: history.c history.h utils.h read.h
	$(CC) $(CFLAGS) -c history.c

utils.o: utils.h constants.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(TARGET) *.o