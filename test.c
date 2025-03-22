#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern void show_alloc_mem(void);

void test_small_allocations() {
    printf("\n----- TESTING SMALL ALLOCATIONS -----\n");
    
    void *ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(50);
        if (!ptrs[i]) {
            printf("Failed to allocate memory!\n");
            return;
        }
        memset(ptrs[i], 'A' + i, 50);
    }
    
    printf("\nAfter small allocations:\n");
    show_alloc_mem();
    
    for (int i = 0; i < 10; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    printf("\nAfter freeing half:\n");
    show_alloc_mem();
    
    for (int i = 1; i < 10; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    printf("\nAfter freeing all small blocks:\n");
    show_alloc_mem();
}

void test_medium_allocations() {
    printf("\n----- TESTING MEDIUM ALLOCATIONS -----\n");
    
    void *ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(500);
        if (!ptrs[i]) {
            printf("Failed to allocate memory!\n");
            return;
        }
        memset(ptrs[i], 'A' + i, 500);
    }
    
    printf("\nAfter medium allocations:\n");
    show_alloc_mem();
    
    for (int i = 0; i < 10; i++) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    printf("\nAfter freeing all medium blocks:\n");
    show_alloc_mem();
}

void test_large_allocations() {
    printf("\n----- TESTING LARGE ALLOCATIONS -----\n");
    
    void *ptrs[5];
    for (int i = 0; i < 5; i++) {
        ptrs[i] = malloc(2000 + i * 1000);
        if (!ptrs[i]) {
            printf("Failed to allocate memory!\n");
            return;
        }
        memset(ptrs[i], 'A' + i, 2000 + i * 1000);
    }
    
    printf("\nAfter large allocations:\n");
    show_alloc_mem();
    
    for (int i = 0; i < 5; i++) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    printf("\nAfter freeing all large blocks:\n");
    show_alloc_mem();
}

void test_realloc() {
    printf("\n----- TESTING REALLOC -----\n");
    
    char *ptr = (char*)malloc(50);
    if (!ptr) {
        printf("Failed to allocate memory!\n");
        return;
    }
    
    strcpy(ptr, "Testing realloc functionality");
    printf("Original string: %s\n", ptr);
    
    ptr = (char*)realloc(ptr, 100);
    if (!ptr) {
        printf("Failed to reallocate memory!\n");
        return;
    }
    
    printf("After realloc, string: %s\n", ptr);
    strcat(ptr, " - with some additional text in the larger buffer");
    printf("Extended string: %s\n", ptr);
    
    printf("\nAfter realloc:\n");
    show_alloc_mem();
    
    free(ptr);
    
    printf("\nAfter freeing realloc'd memory:\n");
    show_alloc_mem();
}

void test_edge_cases() {
    printf("\n----- TESTING EDGE CASES -----\n");
    
    printf("Testing malloc(0)...\n");
    void *ptr = malloc(0);
    printf("malloc(0) returned: %p\n", ptr);
    free(ptr); // Should handle NULL safely
    
    printf("Testing free(NULL)...\n");
    free(NULL); // Should not crash
    printf("free(NULL) completed without crashing\n");
    
    printf("Testing many allocations...\n");
    void *pointers[1000];
    for (int i = 0; i < 1000; i++) {
        pointers[i] = malloc(i % 50 + 1); // 1-50 bytes
        if (!pointers[i]) {
            printf("Failed at allocation %d\n", i);
            break;
        }
    }
    
    for (int i = 0; i < 1000; i++) {
        free(pointers[i]);
    }
    printf("Completed 1000 alloc/free cycles\n");
    
    show_alloc_mem();
}

int main() {
    printf("===== STARTING MALLOC TESTS =====\n");
    
    test_small_allocations();
    test_medium_allocations();
    test_large_allocations();
    test_realloc();
    test_edge_cases();
    
    printf("\n===== ALL TESTS COMPLETED =====\n");
    return 0;
}