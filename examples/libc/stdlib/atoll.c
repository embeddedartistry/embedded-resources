#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

long long atoll(const char *str)
{
	long long val=0;
	bool neg = false;

	while(isspace(*str))
	{
		str++;
	}

	switch (*str) {
		case '-':
			neg = true;
			//Intentional fallthrough
		case '+':
			str++;
	}

	while (isdigit(*str))
	{
		val = (10 * val) + (*str++ - '0');
	}
	return neg ? -val : val;
}
