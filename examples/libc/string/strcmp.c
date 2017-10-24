#include <string.h>

/*
* PJ: my own strcmp implementation
*
* strcmp with short-circuit support: very common when you have const strings
* combined by the compiler.
* Otherwise we compare the strings as normal.
* We bail out when s1 ends (null-term)
*/

int strcmp(const char* s1, const char* s2)
{
	int r = -1;

	if(s1 == s2)
	{
		// short circuit - same string
		return 0;
	}

	// I don't want to panic with a NULL ptr - we'll fall through and fail w/ -1
	if(s1 != NULL && s2 != NULL)
	{
		// iterate through strings until they don't match or s1 ends (null-term)
		for(; *s1 == *s2; ++s1, ++s2)
		{
			if(*s1 == 0)
			{
				r = 0;
				break;
			}
		}

		// handle case where we didn't break early - set return code.
		if(r != 0)
		{
			r = *(char*)s1 - *(char*)s2;
		}
	}

	return r;
}
