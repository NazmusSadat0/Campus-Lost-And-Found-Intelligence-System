CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

OBJECTS = main.o items.o matching.o storage.o

.PHONY: all clean run

all: campus_lost_found

campus_lost_found: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c items.h
	$(CC) $(CFLAGS) -c $< -o $@

run: campus_lost_found
	./campus_lost_found

clean:
	rm -f $(OBJECTS) campus_lost_found