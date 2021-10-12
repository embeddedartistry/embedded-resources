#include <string.h>

int __attribute__((weak)) memcmp(const void* p1, const void* p2, size_t n)
{
	size_t i;

	if(!p1)
	{
		return 1;
	}

	if(!p2)
	{
		return -1;
	}

	/**
	 * p1 and p2 are the same memory? easy peasy! bail out
	 */
	if(p1 == p2)
	{
		return 0;
	}

	// This for loop does the comparing and pointer moving...
	for(i = 0; (i < n) && (*(uint8_t*)p1 == *(uint8_t*)p2);
		i++, p1 = 1 + (uint8_t*)p1, p2 = 1 + (uint8_t*)p2)
	{
		// empty body
	}

	// if i == length, then we have passed the test
	return (i == n) ? 0 : (*(uint8_t*)p1 - *(uint8_t*)p2);
}
