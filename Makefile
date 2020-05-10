# Arena Allocator Library Makefile

CC = gcc
CFLAGS = -Wall -Wextra -Wconversion -Wshadow -std=c99 -pedantic
DEBUG_FLAGS = -ggdb3 -DDEBUG -fsanitize=address -fsanitize=undefined
RELEASE_FLAGS = -O2 -DNDEBUG
TARGET = main
SOURCE = main.c
EXAMPLE_TARGET = example
EXAMPLE_SOURCE = example.c
HEADER = arena.h

# Default target
all: debug

# Debug build with sanitizers
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release build with optimizations
release: CFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

# Build the main target
$(TARGET): $(SOURCE) $(HEADER)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Build the example
$(EXAMPLE_TARGET): $(EXAMPLE_SOURCE) $(HEADER)
	$(CC) $(CFLAGS) -o $(EXAMPLE_TARGET) $(EXAMPLE_SOURCE)

# Run the test program
test: debug
	./$(TARGET)

# Build and run the example
example: CFLAGS += $(DEBUG_FLAGS)
example: $(EXAMPLE_TARGET)
	./$(EXAMPLE_TARGET)

# Run with valgrind (if available)
valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET) $(EXAMPLE_TARGET)

# Install header to system (requires sudo)
install: $(HEADER)
	sudo cp $(HEADER) /usr/local/include/

# Uninstall header from system (requires sudo)
uninstall:
	sudo rm -f /usr/local/include/$(HEADER)

# Format code (requires clang-format)
format:
	clang-format -i $(SOURCE) $(HEADER)

# Static analysis (requires cppcheck)
analyze:
	cppcheck --enable=all --std=c99 $(SOURCE)

# Show help
help:
	@echo "Available targets:"
	@echo "  all      - Build debug version (default)"
	@echo "  debug    - Build with debug flags and sanitizers"
	@echo "  release  - Build optimized release version"
	@echo "  test     - Build and run test program"
	@echo "  example  - Build and run simple example"
	@echo "  valgrind - Run with valgrind memory checker"
	@echo "  clean    - Remove build artifacts"
	@echo "  install  - Install header to /usr/local/include"
	@echo "  uninstall- Remove header from /usr/local/include"
	@echo "  format   - Format code with clang-format"
	@echo "  analyze  - Run static analysis with cppcheck"
	@echo "  help     - Show this help message"

.PHONY: all debug release test valgrind clean install uninstall format analyze help
