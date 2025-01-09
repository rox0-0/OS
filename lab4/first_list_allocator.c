#include "first_list_allocator.h"
#include <math.h>
Allocator* allocator_create(void* memory, size_t size) {
    if (size < sizeof(FreeBlock)) return NULL;

    Allocator* allocator = (Allocator*)memory;
    allocator->memory_start = (char*)memory + sizeof(Allocator);
    allocator->memory_size = size - sizeof(Allocator);
    allocator->free_list = (FreeBlock*)allocator->memory_start;

    allocator->free_list->size = allocator->memory_size;
    allocator->free_list->next = NULL;

    return allocator;
}

void allocator_destroy(Allocator* allocator) {
    (void)allocator;
}

void* allocator_alloc(Allocator* allocator, size_t size) {
    if (size == 0) return NULL;

    FreeBlock* prev = NULL;
    FreeBlock* current = allocator->free_list;

    while (current) {
        if (current->size >= size + sizeof(FreeBlock)) {
            if (current->size > size + sizeof(FreeBlock)) {
                FreeBlock* new_block = (FreeBlock*)((char*)current + sizeof(FreeBlock) + size);
                new_block->size = current->size - size - sizeof(FreeBlock);
                new_block->next = current->next;
                current->next = new_block;
            }

            if (prev) {
                prev->next = current->next;
            } else {
                allocator->free_list = current->next;
            }

            current->size = size;
            return (char*)current + sizeof(FreeBlock);
        }

        prev = current;
        current = current->next;
    }

    return NULL;
}

void allocator_free(Allocator* allocator, void* memory) {
    if (!memory) return;

    FreeBlock* block_to_free = (FreeBlock*)((char*)memory - sizeof(FreeBlock));
    block_to_free->next = allocator->free_list;
    allocator->free_list = block_to_free;
}
