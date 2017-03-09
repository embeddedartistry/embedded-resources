#include "../string.h"

/*
* PJ: my own strncmp implementation
*
* strncmp with short-circuit support: very common when you have const strings
* combined by the compiler.
* Otherwise we compare the strings as normal
* We bail out when s1 ends (null-term) or n bytes have been analyzed
*/

int strncmp(const char *s1, const char *s2, size_t n)
{
	int r = -1;

	if (s1 == s2)
	{
		//short circuit - same string
		return 0;
	}

	//I don't want to panic with a NULL ptr - we'll fall through and fail
	if (s1 != NULL && s2 != NULL)
	{
		//iterate through strings until they don't match, s1 ends, or n == 0
		for (; n && *s1 == *s2; ++s1, ++s2, n--)
		{
			if (*s1 == 0)
			{
				r = 0;
				break;
			}
		}

		// handle case where we didn't break early - set return code.
		if(r != 0)
		{
			r = *(char *)s1 - *(char *)s2;
		}
	}

	return r;
}
