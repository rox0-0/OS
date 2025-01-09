#include "twin_allocator.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
TwinAllocator* twin_allocator_create(void* memory, size_t size) {
    if (size < (1 << MAX_TWIN_ORDER)) return NULL;

    TwinAllocator* allocator = (TwinAllocator*)memory;
    allocator->memory_start = (char*)memory + sizeof(TwinAllocator);
    allocator->memory_size = size - sizeof(TwinAllocator);

    for (int i = 0; i <= MAX_TWIN_ORDER; i++) {
        allocator->free_lists[i] = NULL;
    }

    size_t initial_order = (size_t)log2(size);
    allocator->free_lists[initial_order] = (FreeBlock*)allocator->memory_start;
    allocator->free_lists[initial_order]->size = size;
    allocator->free_lists[initial_order]->next = NULL;

    return allocator;
}

void* twin_allocator_alloc(TwinAllocator* allocator, size_t size) {
    if (size == 0) return NULL;

    size_t order = (size_t)ceil(log2(size + sizeof(FreeBlock)));

    for (size_t i = order; i <= MAX_TWIN_ORDER; i++) {
        if (allocator->free_lists[i]) {
            FreeBlock* block = allocator->free_lists[i];
            allocator->free_lists[i] = block->next;

            while (i > order) {
                i--;
                size_t block_size = 1 << i;
                FreeBlock* twin = (FreeBlock*)((char*)block + block_size);

                twin->size = block_size;  
                twin->next = allocator->free_lists[i];
                allocator->free_lists[i] = twin;
            }


            block->size = (1 << order);
            void* return_ptr = (char*)block + sizeof(FreeBlock);
            *((FreeBlock**)((char*)return_ptr - sizeof(FreeBlock*))) = block;
            return return_ptr;
        }
    }

    return NULL;
}

void twin_allocator_free(TwinAllocator* allocator, void* memory) {
    if (!memory) return;

    FreeBlock* block =  *((FreeBlock**)((char*)memory - sizeof(FreeBlock*)));

    size_t order = (size_t)log2(block->size);
    
    
    FreeBlock** current_list = &allocator->free_lists[order];
    block->next = *current_list;
    *current_list = block;
}
void twin_allocator_destroy(TwinAllocator* allocator) {
    
}
