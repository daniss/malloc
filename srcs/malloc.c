#include "../includes/malloc.h"

t_zone *g_zones[3] = {NULL, NULL, NULL};
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void *mmap_new_zone(size_t size) {
    void *addr;
    
    addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                 MAP_ANON | MAP_PRIVATE, -1, 0);
    if (addr == MAP_FAILED)
        return NULL;
    return addr;
}

t_block *find_block(t_block *blocks, size_t size) {
    t_block *block = blocks;
    
    while (block) {
        if (block->free && block->size >= size)
            return block;
        block = block->next;
    }
    return NULL;
}

e_zone_type get_zone_type(size_t size) {
    if (size <= TINY_MAX)
        return TINY;
    else if (size <= SMALL_MAX)
        return SMALL;
    else
        return LARGE;
}

/* Split a block if it's too big */
void split_block(t_block *block, size_t size) {
    t_block *new_block;
    size_t block_size = block->size;
    
    /* Only split if we can fit a new header + some data */
    if (block_size < size + sizeof(t_block) + 8)
        return;
    
    new_block = (t_block*)((char*)block_to_payload(block) + size);
    new_block->size = block_size - size - sizeof(t_block);
    new_block->free = 1;
    new_block->next = block->next;
    
    block->size = size;
    block->next = new_block;
}

/* Convert block pointer to payload pointer */
void *block_to_payload(t_block *block) {
    return (void*)((char*)block + sizeof(t_block));
}

t_zone *init_zone(size_t zone_size) {
    t_zone *zone;
    t_block *block;

    zone = mmap_new_zone(zone_size);
    if (!zone)
        return NULL;
    
    zone->size = zone_size;
    zone->next = NULL;
    
    block = (t_block*)((char*)zone + sizeof(t_zone));
    block->size = zone_size - sizeof(t_zone) - sizeof(t_block);
    block->free = 1;
    block->next = NULL;
    
    zone->blocks = block;
    return zone;
}

void *malloc(size_t size) {
    t_zone *zone, *new_zone;
    t_block *block;
    e_zone_type type;
    
    /*
        Align the size to 16 bytes to ensure proper memory alignment 
        to define what is memory alignement we can say that it is the way to align the memory in a way that it is easy to access the memory
    */
    size = (size + 15) & ~15;
    
    if (size == 0)
        return NULL;
    
    pthread_mutex_lock(&g_mutex);
    
    type = get_zone_type(size);
    
    if (type == LARGE) {
        /* Allocate a new zone with enough space for the zone header, block header, and data */
        new_zone = mmap_new_zone(sizeof(t_zone) + sizeof(t_block) + size);
        if (!new_zone) {
            pthread_mutex_unlock(&g_mutex);
            return NULL;
        }
        
        new_zone->size = sizeof(t_zone) + sizeof(t_block) + size;
        /* move past the zone header */
        block = (t_block*)((char*)new_zone + sizeof(t_zone));
        block->size = size;
        block->free = 0;
        block->next = NULL;

        new_zone->blocks = block;
        new_zone->next = g_zones[LARGE];
        g_zones[LARGE] = new_zone;
        
        pthread_mutex_unlock(&g_mutex);
        return block_to_payload(block);
    }
    
    size_t zone_size = (type == TINY) ? TINY_ZONE_SIZE : SMALL_ZONE_SIZE;
    zone = g_zones[type];
    
    /* Search through all zones of the appropriate type */
    while (zone) {
        block = find_block(zone->blocks, size);
        if (block) {
            block->free = 0;
            split_block(block, size);
            pthread_mutex_unlock(&g_mutex);
            return block_to_payload(block);
        }
        zone = zone->next;
    }
    
    /* No suitable block found, create a new zone */
    new_zone = init_zone(zone_size);
    if (!new_zone) {
        pthread_mutex_unlock(&g_mutex);
        return NULL;
    }
    
    /* Add the new zone to the beginning of the appropriate list */
    new_zone->next = g_zones[type];
    g_zones[type] = new_zone;
    
    /* Allocate from the newly created zone */
    block = new_zone->blocks;
    block->free = 0;
    split_block(block, size);
    
    pthread_mutex_unlock(&g_mutex);
    return block_to_payload(block);
}