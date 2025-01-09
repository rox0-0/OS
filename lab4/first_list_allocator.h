#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <math.h>
typedef struct FreeBlock {
    size_t size;
    struct FreeBlock* next;
} FreeBlock;

typedef struct Allocator {
    void* memory_start;
    size_t memory_size;
    FreeBlock* free_list;
} Allocator;

Allocator* allocator_create(void* memory, size_t size);
void allocator_destroy(Allocator* allocator);
void* allocator_alloc(Allocator* allocator, size_t size);
void allocator_free(Allocator* allocator, void* memory);

#endif // ALLOCATOR_H
