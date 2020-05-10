/**
 * Arena Allocator Library
 * 
 * A simple, fast arena allocator that manages memory in large blocks.
 * Allocations are fast (just pointer arithmetic) and all memory can be
 * freed at once. Memory is never freed individually - only when the
 * entire arena is reset or freed.
 * 
 * Usage:
 *   #define ARENA_IMPLEMENTATION
 *   #include "arena.h"
 * 
 * Example:
 *   Arena arena = arena_init(1024);
 *   void *ptr = arena_alloc(&arena, 100);
 *   arena_reset(&arena);  // Reset all allocations
 *   arena_free(&arena);   // Free all memory
 */

#ifndef ARENA_H
#define ARENA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#ifndef ARENA_INIT_SIZE
#define ARENA_INIT_SIZE 128
#endif

#ifndef ARENA_ALIGNMENT
#define ARENA_ALIGNMENT 8
#endif

/**
 * Arena structure representing a memory pool.
 * Arenas are organized as a linked list of memory blocks.
 */
typedef struct Arena {
    struct Arena *next;     /**< Next arena block in the chain */
    size_t capacity;        /**< Total capacity of this block */
    size_t size;           /**< Currently used bytes in this block */
    uint8_t *data;         /**< Pointer to the memory block */
} Arena;
	

/**
 * Initialize a new arena with the given capacity.
 * @param capacity Initial capacity in bytes
 * @return Initialized arena structure
 */
Arena arena_init(size_t capacity);

/**
 * Allocate memory from the arena.
 * Memory is aligned to ARENA_ALIGNMENT bytes.
 * @param arena Pointer to the arena
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory, or NULL on failure
 */
void *arena_alloc(Arena *arena, size_t size);

/**
 * Reallocate memory within the arena.
 * Note: This may not reuse the old memory location.
 * @param arena Pointer to the arena
 * @param old_ptr Pointer to existing memory (can be NULL)
 * @param old_size Size of existing memory
 * @param new_size New size required
 * @return Pointer to reallocated memory, or NULL on failure
 */
void *arena_realloc(Arena *arena, void *old_ptr, size_t old_size, size_t new_size);

/**
 * Reset the arena, marking all memory as available for reuse.
 * Does not free the underlying memory blocks.
 * @param arena Pointer to the arena
 */
void arena_reset(Arena *arena);

/**
 * Free all memory associated with the arena.
 * @param arena Pointer to the arena
 */
void arena_free(Arena *arena);

/**
 * Print debug information about the arena.
 * @param arena Pointer to the arena
 */
void arena_print(const Arena *arena);

/**
 * Get the total capacity of all blocks in the arena.
 * @param arena Pointer to the arena
 * @return Total capacity in bytes
 */
size_t arena_total_capacity(const Arena *arena);

/**
 * Get the total used memory across all blocks in the arena.
 * @param arena Pointer to the arena
 * @return Total used memory in bytes
 */
size_t arena_total_used(const Arena *arena);
	
#endif // ARENA_H

#ifdef ARENA_IMPLEMENTATION
#define ARENA_IMPLEMENTATION

#ifndef ARENA_ALLOC
/**
 * Custom malloc wrapper with error handling.
 * @param size Number of bytes to allocate
 * @return Pointer to allocated memory
 */
static void *arena_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Arena: Failed to allocate %zu bytes\n", size);
        exit(1);
    }
    return ptr;
}
#define ARENA_ALLOC arena_malloc
#endif // ARENA_ALLOC

/**
 * Align a size to the specified alignment.
 * @param size Size to align
 * @param alignment Alignment boundary (must be power of 2)
 * @return Aligned size
 */
static inline size_t arena_align_size(size_t size, size_t alignment) {
    return (size + alignment - 1) & ~(alignment - 1);
}


Arena arena_init(size_t capacity) {
    if (capacity == 0) {
        capacity = ARENA_INIT_SIZE;
    }
    
    void *data = ARENA_ALLOC(capacity);
    Arena arena = {
        .capacity = capacity,
        .size = 0,
        .data = (uint8_t*)data,
        .next = NULL,
    };
    return arena;
}

void *arena_alloc(Arena *arena, size_t size) {
    if (arena == NULL || size == 0) {
        return NULL;
    }
    
    // Align the size to prevent alignment issues
    size = arena_align_size(size, ARENA_ALIGNMENT);
    
    Arena *current = arena;
    while (current->size + size > current->capacity) {
        if (current->next == NULL) {
            // Create a new block with at least the requested size
            size_t new_capacity = (arena->capacity > size) ? arena->capacity * 2 : size * 2;
            Arena *next = (Arena*)ARENA_ALLOC(sizeof(Arena));
            Arena new_arena = arena_init(new_capacity);
            *next = new_arena;
            current->next = next;
        }
        current = current->next;
    }

    uint8_t *data = &current->data[current->size];
    current->size += size;
    return data;
}

void *arena_realloc(Arena *arena, void *old_ptr, size_t old_size, size_t new_size) {
    if (arena == NULL) {
        return NULL;
    }
    
    if (new_size == 0) {
        return NULL;
    }
    
    if (old_ptr == NULL) {
        return arena_alloc(arena, new_size);
    }
    
    if (new_size <= old_size) {
        return old_ptr;
    }

    void *new_ptr = arena_alloc(arena, new_size);
    if (new_ptr == NULL) {
        return NULL;
    }
    
    // Copy old data to new location
    memcpy(new_ptr, old_ptr, old_size);
    
    // Note: We don't free old_ptr since arena allocators don't support individual frees
    return new_ptr;
}

void arena_reset(Arena *arena) {
    if (arena == NULL) {
        return;
    }
    
    Arena *current = arena;
    while (current != NULL) {
        current->size = 0;
        current = current->next;
    }
}

void arena_free(Arena *arena) {
    if (arena == NULL) {
        return;
    }
    
    // Free the first block's data
    free(arena->data);
    arena->capacity = 0;
    arena->size = 0;
    arena->data = NULL;
    
    // Free all subsequent blocks
    Arena *current = arena->next;
    while (current != NULL) {
        Arena *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
    arena->next = NULL;
}

void arena_print(const Arena *arena) {
    if (arena == NULL) {
        printf("Arena: NULL\n");
        return;
    }
    
    const Arena *current = arena;
    int block_count = 0;
    printf("Arena blocks: ");
    while (current != NULL) {
        printf("[%d: cap=%zu, used=%zu, ptr=%p] -> ", 
               block_count++, current->capacity, current->size, (void*)current->data);
        current = current->next;
    }
    printf("NULL\n");
    printf("Total blocks: %d, Total capacity: %zu, Total used: %zu\n", 
           block_count, arena_total_capacity(arena), arena_total_used(arena));
}

size_t arena_total_capacity(const Arena *arena) {
    if (arena == NULL) {
        return 0;
    }
    
    size_t total = 0;
    const Arena *current = arena;
    while (current != NULL) {
        total += current->capacity;
        current = current->next;
    }
    return total;
}

size_t arena_total_used(const Arena *arena) {
    if (arena == NULL) {
        return 0;
    }
    
    size_t total = 0;
    const Arena *current = arena;
    while (current != NULL) {
        total += current->size;
        current = current->next;
    }
    return total;
}
	
#endif // ARENA_IMPLEMENTATION