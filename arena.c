/**
 * Arena Allocator Library - Implementation
 * 
 * Implementation of the arena allocator functions.
 */

#include "arena.h"

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
