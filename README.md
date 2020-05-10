# Arena Allocator Library

A simple, fast, and lightweight arena allocator library for C. Arena allocators are excellent for scenarios where you need fast allocations and can free all memory at once, such as parsers, compilers, game engines, and temporary data processing.

## Features

- **Header-only library** - Just include `arena.h` and define `ARENA_IMPLEMENTATION`
- **Fast allocations** - O(1) allocation time using pointer arithmetic
- **Memory alignment** - Automatic alignment to prevent performance issues
- **Dynamic expansion** - Automatically creates new blocks when needed
- **Zero fragmentation** - Linear allocation pattern eliminates fragmentation
- **Simple API** - Easy to use with minimal setup
- **No individual frees** - Memory is freed in bulk for better performance

## Quick Start

```c
#define ARENA_IMPLEMENTATION
#include "arena.h"

int main() {
    // Initialize arena with 1KB
    Arena arena = arena_init(1024);
    
    // Allocate memory
    char *buffer = arena_alloc(&arena, 256);
    int *numbers = arena_alloc(&arena, sizeof(int) * 100);
    
    // Use the memory...
    strcpy(buffer, "Hello, Arena!");
    
    // Reset to reuse memory (optional)
    arena_reset(&arena);
    
    // Free all memory when done
    arena_free(&arena);
    
    return 0;
}
```

## API Reference

### Types

```c
typedef struct Arena {
    struct Arena *next;     // Next arena block in the chain
    size_t capacity;        // Total capacity of this block
    size_t size;           // Currently used bytes in this block
    uint8_t *data;         // Pointer to the memory block
} Arena;
```

### Functions

#### `Arena arena_init(size_t capacity)`
Initialize a new arena with the given capacity.
- **capacity**: Initial capacity in bytes (0 uses `ARENA_INIT_SIZE`)
- **Returns**: Initialized arena structure

#### `void *arena_alloc(Arena *arena, size_t size)`
Allocate memory from the arena. Memory is automatically aligned.
- **arena**: Pointer to the arena
- **size**: Number of bytes to allocate
- **Returns**: Pointer to allocated memory, or NULL on failure

#### `void *arena_realloc(Arena *arena, void *old_ptr, size_t old_size, size_t new_size)`
Reallocate memory within the arena. May not reuse the old memory location.
- **arena**: Pointer to the arena
- **old_ptr**: Pointer to existing memory (can be NULL)
- **old_size**: Size of existing memory
- **new_size**: New size required
- **Returns**: Pointer to reallocated memory, or NULL on failure

#### `void arena_reset(Arena *arena)`
Reset the arena, marking all memory as available for reuse. Does not free underlying memory blocks.
- **arena**: Pointer to the arena

#### `void arena_free(Arena *arena)`
Free all memory associated with the arena.
- **arena**: Pointer to the arena

#### `void arena_print(const Arena *arena)`
Print debug information about the arena.
- **arena**: Pointer to the arena

#### `size_t arena_total_capacity(const Arena *arena)`
Get the total capacity of all blocks in the arena.
- **arena**: Pointer to the arena
- **Returns**: Total capacity in bytes

#### `size_t arena_total_used(const Arena *arena)`
Get the total used memory across all blocks in the arena.
- **arena**: Pointer to the arena
- **Returns**: Total used memory in bytes

## Configuration

You can customize the library behavior by defining these macros before including the header:

```c
#define ARENA_INIT_SIZE 1024        // Default initial size (default: 128)
#define ARENA_ALIGNMENT 16          // Memory alignment (default: 8)
#define ARENA_ALLOC my_malloc       // Custom allocator function
#define ARENA_IMPLEMENTATION        // Required to include implementation
#include "arena.h"
```

## Building

### Simple Build
```bash
cc main.c -o main -Wall -Wextra -ggdb3
```

### With Makefile
```makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = main
SOURCE = main.c

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

.PHONY: clean
```

## Use Cases

Arena allocators are particularly useful for:

- **Parsers and Compilers**: Allocate AST nodes and temporary data
- **Game Engines**: Per-frame allocations that are freed each frame
- **Image/Audio Processing**: Temporary buffers for processing pipelines
- **Network Servers**: Per-request memory pools
- **Data Processing**: Batch processing with bulk cleanup

## Performance Characteristics

- **Allocation**: O(1) - Simple pointer arithmetic
- **Deallocation**: O(1) - Bulk free only
- **Memory overhead**: Minimal - Only arena metadata
- **Fragmentation**: Zero - Linear allocation pattern
- **Cache performance**: Excellent - Sequential memory access

## Limitations

- **No individual frees**: Cannot free specific allocations
- **Memory usage**: May hold onto unused memory until reset/free
- **Not thread-safe**: Requires external synchronization for concurrent access

## License

This library is released into the public domain. See the source code for details.

## Contributing

This is a simple, focused library. If you find bugs or have suggestions for improvements, please feel free to submit issues or pull requests.
