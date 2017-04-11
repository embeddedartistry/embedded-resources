// Pulled from musl libc, locale support removed

#include <ctype.h>

int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}
