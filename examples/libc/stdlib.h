#ifndef __STDLIB_H_
#define __STDLIB_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {

#define NULL 0L

#else // not __cplusplus

#define NULL ((void*)0)

#endif //__cplusplus

int atoi (const char *);
long atol (const char *);
long long atoll (const char *);
double atof (const char *);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __STDLIB_H_
