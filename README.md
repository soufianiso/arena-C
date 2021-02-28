# Arena Allocator

A simple C library for fast memory allocation. Allocate memory quickly and free it all at once.

## Usage

```c
#include "arena.h"

int main() {
    Arena arena = arena_init(1024);
    
    char *buffer = arena_alloc(&arena, 256);
    int *numbers = arena_alloc(&arena, sizeof(int) * 100);
    
    // Use the memory...
    
    arena_free(&arena);  // Free everything at once
    return 0;
}
```

## Building

```bash
make          # Build test program
make example  # Build example program
make run      # Build and run tests
```

## Functions

- `Arena arena_init(size_t capacity)` - Create new arena
- `void *arena_alloc(Arena *arena, size_t size)` - Allocate memory
- `void arena_reset(Arena *arena)` - Reset arena (reuse memory)
- `void arena_free(Arena *arena)` - Free all memory

## When to Use

Good for:
- Parsers and compilers
- Game engines (per-frame data)
- Temporary processing buffers
- Any bulk allocation/deallocation

Not good for:
- Long-lived objects
- Individual memory frees
- Memory-constrained systems

## Inspiration

This library was inspired by the arena allocator concepts from [Handmade Hero](https://handmadehero.org/) by Casey Muratori.
