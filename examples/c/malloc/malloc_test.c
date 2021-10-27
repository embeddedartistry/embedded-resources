#include "malloc_freelist.h"
#include <memory.h>
#include <stdio.h>

#define MALLOC_SIZE (4 * 1024 * 1024)

int main(void)
{
	void* my_block = malloc(MALLOC_SIZE); // For host system demonstration, alloc 4MB block

	printf("Allocated a block on the host, pointer: %p, size %d\n", my_block, MALLOC_SIZE);

	// Initialize our simple freelist with the space.
	// On our embedded platform, this will be the RAM address for your pool and the size you desire.
	malloc_addblock(my_block, MALLOC_SIZE);

	void* t1 = fl_malloc(100);
	printf("Malloc'd from free list: %p\n", t1);

	void* t2 = fl_malloc(4 * 1024);
	printf("Malloc'd from free list: %p\n", t2);

	printf("Putting first pointer back into the pool\n");
	fl_free(t1), t1 = NULL;

	printf("Alloc'ing same size as first pointer\n");
	t1 = fl_malloc(100);

	printf("Malloc'd from free list: %p\n", t1);

	fl_free(t2), t2 = NULL;
	fl_free(t1), t1 = NULL;

	free(my_block);
}
