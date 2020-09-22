# Minimal Makefile for Arena Allocator

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

# Default target
all: test

# Build test program
test: test.c arena.c arena.h
	$(CC) $(CFLAGS) -o test test.c arena.c

# Build example program
example: example.c arena.c arena.h
	$(CC) $(CFLAGS) -o example example.c arena.c

# Run test
run: test
	./test

# Clean build artifacts
clean:
	rm -f test example

.PHONY: all run clean
