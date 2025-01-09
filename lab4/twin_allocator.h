#ifndef TWIN_ALLOCATOR_H
#define TWIN_ALLOCATOR_H

#include "first_list_allocator.h"
#include <stddef.h>
#include <math.h>
#define MAX_TWIN_ORDER 20

typedef struct TwinAllocator {
    void* memory_start;
    size_t memory_size;
    FreeBlock* free_lists[MAX_TWIN_ORDER + 1];
} TwinAllocator;

TwinAllocator* twin_allocator_create(void* memory, size_t size);
void twin_allocator_destroy(TwinAllocator* allocator);
void* twin_allocator_alloc(TwinAllocator* allocator, size_t size);
void twin_allocator_free(TwinAllocator* allocator, void* memory);
void twin_allocator_destroy(TwinAllocator* allocator);
#endif // TWIN_ALLOCATOR_H
