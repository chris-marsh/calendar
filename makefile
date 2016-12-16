CC=gcc
CFLAGS=-std=c89 -Wall -Wextra
SOURCES=acal.c
EXECUTABLE = acal

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES)

.PHONY: clean
clean:
	rm $(EXECUTABLE)
