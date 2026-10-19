CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2

OBJECTS = main.o items.o admin.o matching.o statistics.o file_handler.O
HEADERS = items.h admin.h matching.h statistics.h file_handler.

.PHONY: all clean run

all: campus_lost_found

campus_lost_found: $(OBJECTS)
$(CC)$(CFLAGS) $(OBJECTS) -o$@

%.o: %.c $(HEADERS)
$(CC)$(CFLAGS) -c $< -o$@

run: campus_lost_found
./campus_lost_found

clean:
rm -f $(OBJECTS) campus_lost_found