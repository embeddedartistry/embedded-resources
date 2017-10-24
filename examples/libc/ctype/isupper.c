// Pulled from musl libc, locale support removed

#include <ctype.h>

int isupper(int c)
{
	return (unsigned)c - 'A' < 26;
}
