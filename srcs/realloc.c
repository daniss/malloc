#include "../includes/malloc.h"
#include <string.h>

void *realloc(void *ptr, size_t size) {
    void *new_ptr;
    t_block *block;
    size_t old_size;
    
    if (!ptr)
        return malloc(size);
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    pthread_mutex_lock(&g_mutex);
    
    block = payload_to_block(ptr);
    old_size = block->size;
    
    pthread_mutex_unlock(&g_mutex);

    if (size <= old_size)
        return ptr;
    
    new_ptr = malloc(size);
    if (!new_ptr)
        return NULL;
    
    memcpy(new_ptr, ptr, old_size);
    
    free(ptr);
    
    return new_ptr;
}