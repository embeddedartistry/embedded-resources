#ifndef __MEMORY_H_
#define __MEMORY_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#pragma mark - malloc -
/**
* Using a raw memory address (0xDEADBEEF) and a size,
* add that memory to a free list.  memory will be allocated
* from this pool when people call malloc()
*/
void malloc_addblock(void *addr, size_t size);
void * malloc(size_t size);
void free(void * ptr);

#pragma mark - aligned malloc -
/**
* Allocate memory with at least alignment `align` and size `size`
* Must be freed by calling aligned_free!
*/
void * aligned_malloc(size_t align, size_t size);

//Convenience macro for memalign, the linux API
#define memalign(align, size) aligned_malloc(align, size)

/**
* Free memory that was allocated using aligned_malloc
*/
void aligned_free(void * ptr);

#pragma mark - misc -

void * calloc(size_t num, size_t size);

/**
* realloc(ptr, 0) is a special case that can be handled in two ways. See:
*	 http://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html
* realloc(ptr, 0) returns NULL.
*/
void *realloc(void *ptr, size_t size);

/**
* reallocf is a FreeBSD extension to realloc that frees
* the input pointer if an error occurrs
*
* I do not hand the BSD case of `realloc(ptr,0)` freeing the ptr
*/
void *reallocf(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __MEMORY_H_
