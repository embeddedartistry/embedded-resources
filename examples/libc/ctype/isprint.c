// Pulled from musl libc, locale support removed

#include <ctype.h>

int isprint(int c)
{
	return (unsigned)c - 0x20 < 0x5f;
}
