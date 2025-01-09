CC = gcc
CFLAGS = -Wall -Wextra 
TARGETS = parent child

all: $(TARGETS)

parent: parent.c
	$(CC) $(CFLAGS) -o parent parent.c -lm

child: child.c
	$(CC) $(CFLAGS) -o child child.c -lm

run: all
	@echo "Running parent program..."
	./parent file1

clean:
	rm -f $(TARGETS)

.PHONY: all run clean
