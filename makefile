CC=gcc
CFLAGS=-std=c89 -Wall -Wextra -g
SOURCES=acal.c main.c
EXECUTABLE = acal

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) -I ./

.PHONY: clean
clean:
	rm $(EXECUTABLE)
