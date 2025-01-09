#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <time.h>
#include <sys/mman.h> 
#include <unistd.h> 

#include "first_list_allocator.h"
#include "twin_allocator.h"

#define MEMORY_SIZE (1 << MAX_TWIN_ORDER)
char global_memory[MEMORY_SIZE];

typedef struct Object {
    int id;
    char name[50];
    float value;
} Object;


typedef void* (*allocator_create_t)(void* memory, size_t size);
typedef void (*allocator_destroy_t)(void* allocator);
typedef void* (*allocator_alloc_t)(void* allocator, size_t size);
typedef void (*allocator_free_t)(void* allocator, void* memory);


void* system_allocator_create(void* memory, size_t size) {
     return memory;
}

void system_allocator_destroy(void* allocator) {
    
}

void* system_allocator_alloc(void* allocator, size_t size) {
    if (size == 0) return NULL;

    void* block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
     if (block == MAP_FAILED) {
        return NULL;
    }
    return block;
}

void system_allocator_free(void* allocator, void* memory) {
    if (!memory) return;
    size_t page_size = sysconf(_SC_PAGE_SIZE);
     munmap(memory, page_size);
}


int main(int argc, char *argv[]) {
    allocator_create_t allocator_create = NULL;
    allocator_destroy_t allocator_destroy = NULL;
    allocator_alloc_t allocator_alloc = NULL;
    allocator_free_t allocator_free = NULL;

    void* allocator_lib = NULL;
    char* lib_path = NULL;

    if (argc > 1) {
      lib_path = argv[1];
      allocator_lib = dlopen(lib_path, RTLD_LAZY);
        if (!allocator_lib) {
            fprintf(stderr, "Ошибка загрузки библиотеки %s: %s\n", lib_path, dlerror());
            lib_path = NULL;
        }
    }

    if (lib_path) {
        if (strstr(lib_path, "liballocator.so")) {
              allocator_create = dlsym(allocator_lib, "allocator_create");
            allocator_destroy = dlsym(allocator_lib, "allocator_destroy");
            allocator_alloc = dlsym(allocator_lib, "allocator_alloc");
            allocator_free = dlsym(allocator_lib, "allocator_free");

        } else if (strstr(lib_path, "libtwin_allocator.so")) {
            allocator_create = dlsym(allocator_lib, "twin_allocator_create");
            allocator_destroy = dlsym(allocator_lib, "twin_allocator_destroy");
            allocator_alloc = dlsym(allocator_lib, "twin_allocator_alloc");
            allocator_free = dlsym(allocator_lib, "twin_allocator_free");
        }
    }
     
    if (!allocator_create || !allocator_alloc || !allocator_free) {
         fprintf(stderr, "Ошибка: не удалось найти функции API аллокатора в библиотеке. Используется системный аллокатор.\n");
        allocator_create = (allocator_create_t)system_allocator_create;
        allocator_destroy = (allocator_destroy_t)system_allocator_destroy;
        allocator_alloc = (allocator_alloc_t)system_allocator_alloc;
        allocator_free = (allocator_free_t)system_allocator_free;

    }


    printf("Тестирование аллокатора:\n");
    void* allocator = allocator_create(global_memory, MEMORY_SIZE);
    if (!allocator) {
        fprintf(stderr, "Не удалось создать аллокатор.\n");
        if (allocator_lib) dlclose(allocator_lib);
        return 1;
    }

    clock_t start, end;

    start = clock();
    void* block = allocator_alloc(allocator, 64);
    end = clock();
    double alloc_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Выделен блок: %p, время: %.9f секунд\n", block, alloc_time);

    start = clock();
    allocator_free(allocator, block);
    end = clock();
    double free_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Освобождён блок: %p, время: %.9f секунд\n", block, free_time);

    allocator_destroy(allocator);

     if (allocator_lib) dlclose(allocator_lib);
    return 0;
}
