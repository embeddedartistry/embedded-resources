#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

int atoi(const char* str)
{
	bool neg = false;
	int val = 0;

	switch(*str)
	{
		case '-':
			neg = true;
		// intentional fallthrough to advance str
		case '+':
			str++;
	}

	while(isdigit(*str))
	{
		val = (10 * val) + (*str++ - '0');
	}

	return (neg ? -val : val);
}
