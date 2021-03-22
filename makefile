CC = gcc
CFLAGS = -g -Wall

TARGET = waffsh

all: clean $(TARGET)

$(TARGET): main.o read.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o read.o

main.o: main.c read.h constants.h
	$(CC) $(CFLAGS) -c main.c

read.o: read.c read.h constants.h
	$(CC) $(CFLAGS) -c read.c

clean:
	rm -f $(TARGET) *.o