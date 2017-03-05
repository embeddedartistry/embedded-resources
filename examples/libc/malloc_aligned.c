#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

//contains aligned_malloc and aligned_free prototypes
#include "memory.h"

/**
* Definitions
*/
#pragma mark - Definitions -

//you can #undef this to compile malloc_aligned as a usable library.
//#undef COMPILE_AS_EXAMPLE

/**
* Simple macro for making sure memory addresses are aligned
* to the nearest power of two
*/
#ifndef align_up
#define align_up(num, align) \
	(((num) + ((align) - 1)) & ~((align) - 1))
#endif

//Convenience macro for memalign, the linux API
#define memalign(align, size) aligned_malloc(align, size)

//Number of bytes we're using for storing the aligned pointer offset
typedef uint16_t offset_t;
#define PTR_OFFSET_SZ sizeof(offset_t)

/**
* APIs
*/
#pragma mark - APIs -

/**
* aligned_malloc takes in the requested alignment and size
*	We will call malloc with extra bytes for our header and the offset
*	required to guarantee the desired alignment.
*/
void * aligned_malloc(size_t align, size_t size)
{
	void * ptr = NULL;

	//We want it to be a power of two since align_up operates on powers of two
	assert((align & (align - 1)) == 0);

	if(align && size)
	{
		/*
		 * We know we have to fit an offset value
		 * We also allocate extra bytes to ensure we can meet the alignment
		 */
		uint32_t hdr_size = PTR_OFFSET_SZ + (align - 1);
		void * p = malloc(size + hdr_size);

		if(p)
		{
			/*
			 * Add the offset size to malloc's pointer (we will always store that)
			 * Then align the resulting value to the arget alignment
			 */
			ptr = (void *) align_up(((uintptr_t)p + PTR_OFFSET_SZ), align);

			//Calculate the offset and store it behind our aligned pointer
			*((offset_t *)ptr - 1) = (offset_t)((uintptr_t)ptr - (uintptr_t)p);

		} // else NULL, could not malloc
	} //else NULL, invalid arguments

	return ptr;
}

/**
* aligned_free works like free(), but we work backwards from the returned
* pointer to find the correct offset and pointer location to return to free()
* Note that it is VERY BAD to call free() on an aligned_malloc() pointer.
*/
void aligned_free(void * ptr)
{
	assert(ptr);

	/*
	* Walk backwards from the passed-in pointer to get the pointer offset
	* We convert to an offset_t pointer and rely on pointer math to get the data
	*/
	offset_t offset = *((offset_t *)ptr - 1);

	/*
	* Once we have the offset, we can get our original pointer and call free
	*/
	void * p = (void *)((uint8_t *)ptr - offset);
	free(p);
}
