#ifndef __STDLIB_H_
#define __STDLIB_H_

#include <stddef.h>
#include "malloc_freelist.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#pragma mark - definitions & types -

typedef struct {
	int quot;
	int rem;
} div_t;

typedef struct {
	long quot;
	long rem;
} ldiv_t;

typedef struct {
	long long quot;
	long long rem;
} lldiv_t;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define RAND_MAX (0x7fffffff)

#pragma mark - ascii-to-x -

int atoi (const char *);
long atol (const char *);
long long atoll (const char *);
double atof (const char *);

#pragma mark - str-to-x -

double strtod (const char *__restrict, char **__restrict);
long strtol (const char *__restrict, char **__restrict, int);
unsigned long strtoul (const char *__restrict, char **__restrict, int);
long long strtoll (const char *__restrict, char **__restrict, int);
unsigned long long strtoull (const char *__restrict, char **__restrict, int);

#pragma mark - math -

int abs (int);
long labs (long);
long long llabs (long long);

div_t div (int, int);
ldiv_t ldiv (long, long);
lldiv_t lldiv (long long, long long);

int rand (void);
void srand (unsigned);

#pragma mark - sorting -
void *bsearch (const void *, const void *, size_t, size_t, int (*)(const void *, const void *));

#pragma mark - memory -

void * malloc(size_t size);
void free(void * ptr);

void * calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);
void *reallocf(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __STDLIB_H_
