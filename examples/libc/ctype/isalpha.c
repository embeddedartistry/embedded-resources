// Pulled from musl libc, locale support removed

#include <ctype.h>

int isalpha(int c)
{
	return ((unsigned)c | 32) - 'a' < 26;
}
