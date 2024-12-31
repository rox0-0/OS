#include "memory_manager.h"

MemoryManager* create_manager(void* memory_pool, size_t pool_size) {
    if (pool_size < sizeof(FreeNode)) return NULL;

    MemoryManager* manager = (MemoryManager*)memory_pool;
    manager->start_address = (char*)memory_pool + sizeof(MemoryManager);
    manager->total_size = pool_size - sizeof(MemoryManager);
    manager->free_blocks = (FreeNode*)manager->start_address;

    manager->free_blocks->block_size = manager->total_size;
    manager->free_blocks->next_node = NULL;

    return manager;
}

void destroy_manager(MemoryManager* manager) {
    (void)manager;
}

void* allocate_block(MemoryManager* manager, size_t size) {
    if (size == 0) return NULL;

    FreeNode* previous = NULL;
    FreeNode* current = manager->free_blocks;

    while (current) {
        if (current->block_size >= size + sizeof(FreeNode)) {
            if (current->block_size > size + sizeof(FreeNode)) {
                FreeNode* new_block = (FreeNode*)((char*)current + sizeof(FreeNode) + size);
                new_block->block_size = current->block_size - size - sizeof(FreeNode);
                new_block->next_node = current->next_node;
                current->next_node = new_block;
            }

            if (previous) {
                previous->next_node = current->next_node;
            } else {
                manager->free_blocks = current->next_node;
            }

            current->block_size = size;
            return (char*)current + sizeof(FreeNode);
        }

        previous = current;
        current = current->next_node;
    }

    return NULL;
}

void free_block(MemoryManager* manager, void* block) {
    if (!block) return;

    FreeNode* released_block = (FreeNode*)((char*)block - sizeof(FreeNode));
    released_block->next_node = manager->free_blocks;
    manager->free_blocks = released_block;
}
