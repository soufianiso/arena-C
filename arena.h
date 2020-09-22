/**
 * Arena Allocator Library
 * 
 * A simple, fast arena allocator that manages memory in large blocks.
 * Allocations are fast (just pointer arithmetic) and all memory can be
 * freed at once. Memory is never freed individually - only when the
 * entire arena is reset or freed.
 * 
 * Usage:
 *   #include "arena.h"
 *   // Link with arena.c when compiling
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
