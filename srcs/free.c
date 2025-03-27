#include "../includes/malloc.h"

t_block *payload_to_block(void *ptr) {
    return (t_block*)((char*)ptr - sizeof(t_block));
}

t_zone *find_zone_of_ptr(void *ptr) {
    t_zone *zone;
    int i;
    
    for (i = 0; i < 3; i++) {
        zone = g_zones[i];
        while (zone) {
            if ((char*)ptr >= (char*)zone && (char*)ptr < ((char*)zone + zone->size))
                return zone;
            zone = zone->next;
        }
    }
    return NULL;
}

int is_zone_free(t_zone *zone) {
    t_block *block = zone->blocks;
    
    while (block) {
        if (!block->free)
            return 0;
        block = block->next;
    }
    return 1;
}

int is_valid_block_ptr(void *ptr, t_zone *zone) {
    t_block *block = zone->blocks;

    while (block) {
        if ((void*)((char*)block + sizeof(t_block)) == ptr)
            return 1;
        block = block->next;
    }
    return 0;
}

void unmap_zone(t_zone *zone, e_zone_type type) {
    t_zone *prev = NULL;
    t_zone *curr = g_zones[type];
    
    while (curr && curr != zone) {
        prev = curr;
        curr = curr->next;
    }
    
    if (curr) {
        if (prev)
            prev->next = curr->next;
        else
            g_zones[type] = curr->next;
        munmap(zone, zone->size);
    }
}

void free(void *ptr) {
    t_zone *zone;
    t_block *block;
    e_zone_type type;
    t_block *prev = NULL;

    if (!ptr)
        return;
    pthread_mutex_lock(&g_mutex);

    zone = find_zone_of_ptr(ptr);
    if (!zone || !is_valid_block_ptr(ptr, zone)) {
        pthread_mutex_unlock(&g_mutex);
        return;
    }

    t_block *curr = zone->blocks;
    block = payload_to_block(ptr);

    while (curr && curr != block) {
        prev = curr;
        curr = curr->next;
    }

    block->free = 1;

    t_block *next = block->next;
    if (next && next->free) {
        block->size += sizeof(t_block) + next->size;
        block->next = next->next;
    }

    if (prev && prev->free) {
        prev->size += sizeof(t_block) + block->size;
        prev->next = block->next;
        block = prev;
    }

    if (block->size <= TINY_MAX)
        type = TINY;
    else if (block->size <= SMALL_MAX)
        type = SMALL;
    else
        type = LARGE;

    if (type == LARGE) {
        unmap_zone(zone, LARGE);
    }
    else if (is_zone_free(zone)) {
        unmap_zone(zone, type);
    }

    pthread_mutex_unlock(&g_mutex);
}
