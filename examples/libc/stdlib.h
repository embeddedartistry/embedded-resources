#ifndef __STDLIB_H_
#define __STDLIB_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {

#define NULL 0L

#else // not __cplusplus

#define NULL ((void*)0)

#endif //__cplusplus


#pragma mark - ascii-to-x -

int atoi (const char *);
long atol (const char *);
long long atoll (const char *);
double atof (const char *);

#pragma mark - memory -

void * calloc(size_t num, size_t size);

/**
* realloc(ptr, 0) is a special case that can be handled in two ways. See:
*	 http://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html
* realloc(ptr, 0) returns NULL.
*/
void *realloc(void *ptr, size_t size);

/**
* reallocf is a FreeBSD extension to realloc that frees
* the input pointer if an error occurrs
*
* I do not hand the BSD case of `realloc(ptr,0)` freeing the ptr
*/
void *reallocf(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __STDLIB_H_
