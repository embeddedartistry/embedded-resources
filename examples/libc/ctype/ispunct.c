// Pulled from musl libc, locale support removed

#include <ctype.h>

int ispunct(int c)
{
	return isgraph(c) && !isalnum(c);
}
