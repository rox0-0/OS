#ifndef BUDDY_MANAGER_H
#define BUDDY_MANAGER_H

#include "memory_manager.h"
#include <stddef.h>

#define MAX_BUDDY_LEVEL 20

typedef struct BuddyManager {
    void* base_address;
    size_t total_capacity;
    FreeNode* levels[MAX_BUDDY_LEVEL + 1];
} BuddyManager;

BuddyManager* initialize_buddy(void* memory_pool, size_t pool_size);
void* allocate_buddy_block(BuddyManager* manager, size_t size);
void release_buddy_block(BuddyManager* manager, void* block);

#endif  BUDDY_MANAGER_H
