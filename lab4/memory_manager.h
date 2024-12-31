#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

typedef struct FreeNode {
    size_t block_size;
    struct FreeNode* next_node;
} FreeNode;

typedef struct MemoryManager {
    void* start_address;
    size_t total_size;
    FreeNode* free_blocks;
} MemoryManager;

MemoryManager* create_manager(void* memory_pool, size_t pool_size);
void destroy_manager(MemoryManager* manager);
void* allocate_block(MemoryManager* manager, size_t size);
void free_block(MemoryManager* manager, void* block);

#endif  MEMORY_MANAGER_H
