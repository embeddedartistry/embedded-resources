#include "memory.h"
#include "string.h"
#include <stdbool.h>

/*
 * This is sqrt(SIZE_MAX+1), as s1*s2 <= SIZE_MAX
 * if both s1 < MUL_NO_OVERFLOW and s2 < MUL_NO_OVERFLOW
 */
#define MUL_NO_OVERFLOW (1UL << (sizeof(size_t) * 4))

void *calloc(size_t num, size_t size)
{
	/* num * size unsigned integer wrapping check */
	if ((num >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) &&
	    num > 0 && SIZE_MAX / num < size) {
		return NULL;
	}

	size_t sz = num * size;

	void *ptr = malloc(sz);
	if (ptr) {
		memset(ptr, 0, sz);
	}
	return ptr;
}

void *realloc(void *ptr, size_t size)
{
	if (!ptr) {
		return malloc(size);
	}

	void *new_data = malloc(size);
	if (new_data) {
		memcpy(new_data, ptr, size); //TODO: unsafe copy...
		free(ptr); //we always move the data. free.
	}

	return new_data;
}

void *reallocf(void *ptr, size_t size)
{
	void *p = realloc(ptr, size);

	if ((p == NULL) && (ptr != NULL))
	{
		free(ptr);
	}

	return p;
}
