// Pulled from musl libc, locale support removed

#include <ctype.h>

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}
