# Compiler and flags for building the shell
CC := gcc
CPPFLAGS := -D_POSIX_C_SOURCE=200809L
CFLAGS := -Wall -Wextra -pedantic -std=c11

# Default target builds the shell executable
all: shell

shell: shell.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

run: shell
	./shell

clean:
	rm -f shell

.PHONY: all run clean
