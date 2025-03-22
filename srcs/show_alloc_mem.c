#include "../includes/malloc.h"
#include <unistd.h>

void print_addr(void *addr) {
    static const char hex_digits[] = "0123456789ABCDEF";
    char addr_str[20] = "0x";
    unsigned long n = (unsigned long)addr;
    int i = 2;
    
    while (n > 0) {
        addr_str[i++] = hex_digits[n % 16];
        n /= 16;
    }
    addr_str[i] = '\0';
    
    int start = 2;
    int end = i - 1;
    char temp;
    while (start < end) {
        temp = addr_str[start];
        addr_str[start] = addr_str[end];
        addr_str[end] = temp;
        start++;
        end--;
    }
    
    write(1, addr_str, i);
}

void print_size(size_t size) {
    char buffer[20];
    int i = 0;
    
    if (size == 0) {
        write(1, "0", 1);
        return;
    }
    
    while (size > 0) {
        buffer[i++] = '0' + (size % 10);
        size /= 10;
    }
    
    int start = 0;
    int end = i - 1;
    char temp;
    while (start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
    
    write(1, buffer, i);
}

void show_alloc_mem(void) {
    t_zone *zone;
    t_block *block;
    size_t total = 0;
    
    pthread_mutex_lock(&g_mutex);
    
    write(1, "TINY : ", 7);
    zone = g_zones[TINY];
    if (zone)
        print_addr(zone);
    write(1, "\n", 1);
    
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (!block->free) {
                print_addr(block_to_payload(block));
                write(1, " - ", 3);
                print_addr((char*)block_to_payload(block) + block->size);
                write(1, " : ", 3);
                print_size(block->size);
                write(1, " bytes\n", 7);
                total += block->size;
            }
            block = block->next;
        }
        zone = zone->next;
    }
    
    write(1, "SMALL : ", 8);
    zone = g_zones[SMALL];
    if (zone)
        print_addr(zone);
    write(1, "\n", 1);
    
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (!block->free) {
                print_addr(block_to_payload(block));
                write(1, " - ", 3);
                print_addr((char*)block_to_payload(block) + block->size);
                write(1, " : ", 3);
                print_size(block->size);
                write(1, " bytes\n", 7);
                total += block->size;
            }
            block = block->next;
        }
        zone = zone->next;
    }
    
    write(1, "LARGE : ", 8);
    zone = g_zones[LARGE];
    if (zone)
        print_addr(zone);
    write(1, "\n", 1);
    
    while (zone) {
        block = zone->blocks;
        while (block) {
            if (!block->free) {
                print_addr(block_to_payload(block));
                write(1, " - ", 3);
                print_addr((char*)block_to_payload(block) + block->size);
                write(1, " : ", 3);
                print_size(block->size);
                write(1, " bytes\n", 7);
                total += block->size;
            }
            block = block->next;
        }
        zone = zone->next;
    }
    
    write(1, "Total : ", 8);
    print_size(total);
    write(1, " bytes\n", 7);
    
    pthread_mutex_unlock(&g_mutex);
}