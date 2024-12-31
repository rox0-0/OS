#include "allocator.h"
#include "buddy_allocator.h"
#include <stdio.h>
#include <time.h>
#include <dlfcn.h>

#define POOL_SIZE (1 << MAX_BUDDY_LEVEL)
char shared_memory[POOL_SIZE];

typedef struct Item {
    int identifier;
    char label[50];
    float data;
} Item;
int main() {
    void* basic_allocator_lib = dlopen("./libbasic_allocator.so", RTLD_LAZY);
    if (!basic_allocator_lib) {
        fprintf(stderr, "Failed to load libbasic_allocator.so: %s\n", dlerror());
        return 1;
    }

    void* buddy_allocator_lib = dlopen("./libbuddy_allocator.so", RTLD_LAZY);
    if (!buddy_allocator_lib) {
        fprintf(stderr, "Failed to load libbuddy_allocator.so: %s\n", dlerror());
        dlclose(basic_allocator_lib);
        return 1;
    }

    printf("Testing Basic Memory Manager:\n");

    MemoryManager* simple_manager = create_manager(shared_memory, POOL_SIZE);

    Item* item1 = allocate_block(simple_manager, sizeof(Item));
    Item* item2 = allocate_block(simple_manager, sizeof(Item));
    Item* item3 = allocate_block(simple_manager, sizeof(Item));

    if (item1) {
        item1->identifier = 101;
        snprintf(item1->label, sizeof(item1->label), "Item A");
        item1->data = 3.14;
    }

    if (item2) {
        item2->identifier = 102;
        snprintf(item2->label, sizeof(item2->label), "Item B");
        item2->data = 1.618;
    }

    if (item3) {
        item3->identifier = 103;
        snprintf(item3->label, sizeof(item3->label), "Item C");
        item3->data = 2.718;
    }

    free_block(simple_manager, item1);
    free_block(simple_manager, item2);
    free_block(simple_manager, item3);

    printf("\nTesting Buddy Memory Manager:\n");

    BuddyManager* buddy_manager = initialize_buddy(shared_memory, POOL_SIZE);

    Item* buddy_item1 = allocate_buddy_block(buddy_manager, sizeof(Item));
    Item* buddy_item2 = allocate_buddy_block(buddy_manager, sizeof(Item));
    Item* buddy_item3 = allocate_buddy_block(buddy_manager, sizeof(Item));

    if (buddy_item1) {
        buddy_item1->identifier = 201;
        snprintf(buddy_item1->label, sizeof(buddy_item1->label), "Buddy A");
        buddy_item1->data = 42.0;
    }

    if (buddy_item2) {
        buddy_item2->identifier = 202;
        snprintf(buddy_item2->label, sizeof(buddy_item2->label), "Buddy B");
        buddy_item2->data = 84.0;
    }

    if (buddy_item3) {
        buddy_item3->identifier = 203;
        snprintf(buddy_item3->label, sizeof(buddy_item3->label), "Buddy C");
        buddy_item3->data = 168.0;
    }

    release_buddy_block(buddy_manager, buddy_item1);
    release_buddy_block(buddy_manager, buddy_item2);
    release_buddy_block(buddy_manager, buddy_item3);

    dlclose(basic_allocator_lib);
    dlclose(buddy_allocator_lib);

    return 0;
}
