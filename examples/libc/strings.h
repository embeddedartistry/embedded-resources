#ifndef __STRINGS_H_
#define __STRINGS_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

	int fls(int mask);
	int flsl(long mask);
	int flsll(long long mask);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __STRINGS_H_
