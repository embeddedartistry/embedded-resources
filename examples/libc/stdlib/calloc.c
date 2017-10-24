#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * This is sqrt(SIZE_MAX+1), as s1*s2 <= SIZE_MAX
 * if both s1 < MUL_NO_OVERFLOW and s2 < MUL_NO_OVERFLOW
 */
#define MUL_NO_OVERFLOW (1UL << (sizeof(size_t) * 4))

void* calloc(size_t num, size_t size)
{
	/* num * size unsigned integer wrapping check */
	if((num >= MUL_NO_OVERFLOW || size >= MUL_NO_OVERFLOW) && num > 0 && SIZE_MAX / num < size)
	{
		return NULL;
	}

	size_t total_size = num * size;
	void* ptr = malloc(total_size);

	if(ptr)
	{
		memset(ptr, 0, total_size);
	}

	return ptr;
}
