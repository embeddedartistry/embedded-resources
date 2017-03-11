#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int atoi(const char *str)
{
	bool neg = false;
	int val = 0;

	switch(*str)
	{
		case '-':
			neg = true;
			//intentional fallthrough to advance str
		case '+':
			str++;
	}

	while (isdigit(*str)) {
		val = (10 * val) + (*str++ - '0');
	}

	return (neg ? -val : val);
}
