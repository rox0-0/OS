CC = gcc
CFLAGS = -shared -fPIC -Wall -O2
LDFLAGS = -ldl -rdynamic -Wl,-rpath,.
LIBFLAGS = -L. -lallocator -ltwin_allocator -lm

# Названия библиотек и исходных файлов
BASIC_ALLOCATOR_SRC = first_list_allocator.c 
BUDDY_ALLOCATOR_SRC = twin_allocator.c
MAIN_SRC = main.c

BASIC_ALLOCATOR_LIB = liballocator.so
BUDDY_ALLOCATOR_LIB = libtwin_allocator.so
MAIN_EXEC = memory_allocator

.PHONY: all clean

all: $(BASIC_ALLOCATOR_LIB) $(BUDDY_ALLOCATOR_LIB) $(MAIN_EXEC)

$(BASIC_ALLOCATOR_LIB): $(BASIC_ALLOCATOR_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(BUDDY_ALLOCATOR_LIB): $(BUDDY_ALLOCATOR_SRC)
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(MAIN_EXEC): $(MAIN_SRC) $(BASIC_ALLOCATOR_LIB) $(BUDDY_ALLOCATOR_LIB)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBFLAGS)

clean:
	rm -f $(BASIC_ALLOCATOR_LIB) $(BUDDY_ALLOCATOR_LIB) $(MAIN_EXEC)
