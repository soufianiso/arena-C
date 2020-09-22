/**
 * Simple example demonstrating arena allocator usage
 */

#include "arena.h"

int main(void) {
    printf("Arena Allocator Example\n");
    printf("=======================\n\n");
    
    // Initialize arena with 1KB
    Arena arena = arena_init(1024);
    printf("Initialized arena with 1KB capacity\n");
    
    // Allocate some memory
    char *message = arena_alloc(&arena, 64);
    int *numbers = arena_alloc(&arena, sizeof(int) * 10);
    
    if (message && numbers) {
        // Use the allocated memory
        strcpy(message, "Hello from arena allocator!");
        for (int i = 0; i < 10; i++) {
            numbers[i] = i * i;
        }
        
        printf("Message: %s\n", message);
        printf("Numbers: ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n\n");
        
        // Show arena status
        arena_print(&arena);
        printf("\n");
    }
    
    // Reset arena (marks all memory as available for reuse)
    printf("Resetting arena...\n");
    arena_reset(&arena);
    arena_print(&arena);
    printf("\n");
    
    // Allocate again after reset
    char *new_message = arena_alloc(&arena, 32);
    if (new_message) {
        strcpy(new_message, "Reused memory!");
        printf("New message: %s\n", new_message);
        arena_print(&arena);
        printf("\n");
    }
    
    // Clean up - free all memory
    printf("Freeing arena...\n");
    arena_free(&arena);
    arena_print(&arena);
    
    return 0;
}
