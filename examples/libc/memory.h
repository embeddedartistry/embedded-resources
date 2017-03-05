#ifndef __MEMORY_H_
#define __MEMORY_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


/**
* Using a raw memory address (0xDEADBEEF) and a size,
* add that memory to a free list.  memory will be allocated
* from this pool when people call malloc()
*/
void malloc_addblock(void *addr, size_t size);

void * malloc(size_t size);
void free(void * ptr);

/**
* Allocate memory with at least alignment `align` and size `size`
* Must be freed by calling aligned_free!
*/
void * aligned_malloc(size_t align, size_t size);

/**
* Free memory that was allocated using aligned_malloc
*/
void aligned_free(void * ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __MEMORY_H_
