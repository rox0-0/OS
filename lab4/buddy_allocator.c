#include "buddy_manager.h"
#include <math.h>
BuddyManager* initialize_buddy(void* memory_pool, size_t pool_size) {
    if (pool_size < (1 << MAX_BUDDY_LEVEL)) return NULL;

    BuddyManager* manager = (BuddyManager*)memory_pool;
    manager->base_address = (char*)memory_pool + sizeof(BuddyManager);
    manager->total_capacity = pool_size - sizeof(BuddyManager);

    for (int i = 0; i <= MAX_BUDDY_LEVEL; i++) {
        manager->levels[i] = NULL;
    }

    size_t initial_level = (size_t)log2(pool_size);
    manager->levels[initial_level] = (FreeNode*)manager->base_address;
    manager->levels[initial_level]->block_size = pool_size;
    manager->levels[initial_level]->next_node = NULL;

    return manager;
}

void* allocate_buddy_block(BuddyManager* manager, size_t size) {
    if (size == 0) return NULL;

    size_t level = (size_t)ceil(log2(size + sizeof(FreeNode)));

    for (size_t i = level; i <= MAX_BUDDY_LEVEL; i++) {
        if (manager->levels[i]) {
            FreeNode* block = manager->levels[i];
            manager->levels[i] = block->next_node;

            while (i > level) {
                i--;
                size_t split_size = 1 << i;
                FreeNode* buddy = (FreeNode*)((char*)block + split_size);
                buddy->block_size = split_size;
                buddy->next_node = manager->levels[i];
                manager->levels[i] = buddy;
            }

            block->block_size = (1 << level);
            return (char*)block + sizeof(FreeNode);
        }
    }

    return NULL;
}

void release_buddy_block(BuddyManager* manager, void* block) {
    if (!block) return;

    FreeNode* node = (FreeNode*)((char*)block - sizeof(FreeNode));
    size_t level = (size_t)log2(node->block_size);

    FreeNode** current_level = &manager->levels[level];
    node->next_node = *current_level;
    *current_level = node;
}
