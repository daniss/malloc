#ifndef FT_MALLOC_H
# define FT_MALLOC_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stddef.h>

/*
 * Memory allocation categories:
 * TINY: 1 to 128 bytes
 * SMALL: 129 to 1024 bytes
 * LARGE: 1025+ bytes
 */
#define TINY_MAX 128
#define SMALL_MAX 1024

size_t get_page_size(void);

#define TINY_ZONE_SIZE (get_page_size() * 4)  /* 16KB for 4KB pages */
#define SMALL_ZONE_SIZE (get_page_size() * 25) /* 100KB for 4KB pages */

/* Memory block metadata */
typedef struct s_block {
    size_t size;
    int free;
    struct s_block *next;
} t_block;

typedef struct s_zone {
    size_t size;
    t_block *blocks;
    struct s_zone *next;
} t_zone;

extern t_zone *g_zones[3];
extern pthread_mutex_t g_mutex;

typedef enum {
    TINY = 0,
    SMALL = 1,
    LARGE = 2
} e_zone_type;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void);

void *mmap_new_zone(size_t size);
t_block *find_block(t_block *blocks, size_t size);
e_zone_type get_zone_type(size_t size);
void *block_to_payload(t_block *block);
t_block *payload_to_block(void *ptr);
void split_block(t_block *block, size_t size);
t_zone *find_zone_of_ptr(void *ptr);

#endif