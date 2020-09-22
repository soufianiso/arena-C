#include "arena.h"

static void test_basic_allocation(void) {
    printf(" Testing Basic Allocation \n");
    Arena arena = arena_init(ARENA_INIT_SIZE);
    
    void *ptr1 = arena_alloc(&arena, 18);
    char *ptr2 = arena_alloc(&arena, 11);
    void *ptr3 = arena_alloc(&arena, 64);
    
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr3 != NULL);
    
    printf("Allocated 3 blocks:\n");
    arena_print(&arena);
    
    if (ptr2) {
        strcpy(ptr2, "Hello!");
        printf("String in ptr2: %s\n", ptr2);
    }
    
    arena_free(&arena);
    printf("After free:\n");
    arena_print(&arena);
    printf("\n");
}

static void test_large_allocations(void) {
    printf(" Testing Large Allocations (Multiple Blocks) \n");
    Arena arena = arena_init(ARENA_INIT_SIZE);
    
    // These should trigger multiple arena blocks
    void *ptr1 = arena_alloc(&arena, 100);
    void *ptr2 = arena_alloc(&arena, 200);
    void *ptr3 = arena_alloc(&arena, 500);
    void *ptr4 = arena_alloc(&arena, 1000);
    
    // Verify all allocations succeeded
    assert(ptr1 != NULL);
    assert(ptr2 != NULL);
    assert(ptr3 != NULL);
    assert(ptr4 != NULL);
    
    printf("After large allocations:\n");
    arena_print(&arena);
    
    arena_free(&arena);
    printf("\n");
}

static void test_realloc(void) {
    printf("=== Testing Reallocation ===\n");
    Arena arena = arena_init(256);
    
    // Initial allocation
    char *ptr = arena_alloc(&arena, 10);
    if (ptr) {
        strcpy(ptr, "Small");
        printf("Initial string: %s\n", ptr);
    }
    
    // Reallocate to larger size
    ptr = arena_realloc(&arena, ptr, 10, 50);
    if (ptr) {
        strcat(ptr, " -> Now Larger!");
        printf("After realloc: %s\n", ptr);
    }
    
    arena_print(&arena);
    arena_free(&arena);
    printf("\n");
}

static void test_reset(void) {
    printf("=== Testing Arena Reset ===\n");
    Arena arena = arena_init(256);
    
    // Allocate some memory
    arena_alloc(&arena, 50);
    arena_alloc(&arena, 100);
    printf("Before reset:\n");
    arena_print(&arena);
    
    // Reset and allocate again
    arena_reset(&arena);
    printf("After reset:\n");
    arena_print(&arena);
    
    // Allocate again after reset
    arena_alloc(&arena, 30);
    printf("After new allocation:\n");
    arena_print(&arena);
    
    arena_free(&arena);
    printf("\n");
}

static void test_alignment(void) {
    printf("=== Testing Memory Alignment ===\n");
    Arena arena = arena_init(256);
    
    // Allocate various sizes to test alignment
    void *ptr1 = arena_alloc(&arena, 1);   // Should be aligned
    void *ptr2 = arena_alloc(&arena, 3);   // Should be aligned
    void *ptr3 = arena_alloc(&arena, 7);   // Should be aligned
    void *ptr4 = arena_alloc(&arena, 15);  // Should be aligned
    
    printf("Alignment test (should be %d-byte aligned):\n", ARENA_ALIGNMENT);
    
    printf("ptr1: %p (aligned: ", ptr1);
    if ((uintptr_t)ptr1 % ARENA_ALIGNMENT == 0) {
        printf("yes)\n");
    } else {
        printf("no)\n");
    }
    
    printf("ptr2: %p (aligned: ", ptr2);
    if ((uintptr_t)ptr2 % ARENA_ALIGNMENT == 0) {
        printf("yes)\n");
    } else {
        printf("no)\n");
    }
    
    printf("ptr3: %p (aligned: ", ptr3);
    if ((uintptr_t)ptr3 % ARENA_ALIGNMENT == 0) {
        printf("yes)\n");
    } else {
        printf("no)\n");
    }
    
    printf("ptr4: %p (aligned: ", ptr4);
    if ((uintptr_t)ptr4 % ARENA_ALIGNMENT == 0) {
        printf("yes)\n");
    } else {
        printf("no)\n");
    }
    
    arena_print(&arena);
    arena_free(&arena);
    printf("\n");
}

int main(void) {
    printf("Arena Allocator Library Test Suite\n");
    
    test_basic_allocation();
    test_large_allocations();
    test_realloc();
    test_reset();
    test_alignment();
    
    printf("All tests completed!\n");
    return 0;
}
