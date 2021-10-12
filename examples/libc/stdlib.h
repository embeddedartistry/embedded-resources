#ifndef __STDLIB_H_
#define __STDLIB_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

#pragma mark - definitions & types -

	typedef struct
	{
		int quot;
		int rem;
	} div_t;

	typedef struct
	{
		long quot;
		long rem;
	} ldiv_t;

	typedef struct
	{
		long long quot;
		long long rem;
	} lldiv_t;

	typedef struct
	{
		intmax_t quot;
		intmax_t rem;
	} imaxdiv_t;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define RAND_MAX (0x7fffffff)

#pragma mark - ascii-to-x -

	int atoi(const char*);
	long atol(const char*);
	long long atoll(const char*);
	double atof(const char*);

#pragma mark - str-to-x -

	float strtof(const char* __restrict, char** __restrict);
	double strtod(const char* __restrict, char** __restrict);
	long strtol(const char* __restrict, char** __restrict, int);
	unsigned long strtoul(const char* __restrict, char** __restrict, int);
	long long strtoll(const char* __restrict, char** __restrict, int);
	unsigned long long strtoull(const char* __restrict, char** __restrict, int);

#pragma mark - math -

	int abs(int);
	long labs(long);
	long long llabs(long long);
	intmax_t imaxabs(intmax_t j);

	div_t div(int, int);
	ldiv_t ldiv(long, long);
	lldiv_t lldiv(long long, long long);
	imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom);

	int rand(void);
	void srand(unsigned);

#pragma mark - random number generation -
	int rand_r(unsigned int* ctx);
	int rand(void);
	void srand(unsigned seed);

#pragma mark - sorting -
	int heapsort(void* vbase, size_t nmemb, size_t size, int (*compar)(const void*, const void*));
	void* bsearch(const void*, const void*, size_t, size_t, int (*)(const void*, const void*));
	void qsort_r(void* a, size_t n, size_t es, void* thunk,
				 int (*cmp)(void*, const void*, const void*));
	void qsort(void* a, size_t n, size_t es, int (*compar)(const void*, const void*));

#pragma mark - memory -

	/**
	 * Allocate memory
	 */
	void* malloc(size_t size);

	/**
	 * Free previously allocated memory
	 */
	void free(void* ptr);

	void* calloc(size_t num, size_t size);

	/**
	 * realloc(ptr, 0) is a special case that can be handled in two ways. See:
	 *	 http://pubs.opengroup.org/onlinepubs/9699919799/functions/realloc.html
	 * realloc(ptr, 0) returns NULL.
	 */
	void* realloc(void* ptr, size_t size);

	/**
	 * reallocf is a FreeBSD extension to realloc that frees
	 * the input pointer if an error occurrs
	 *
	 * I do not hand the BSD case of `realloc(ptr,0)` freeing the ptr
	 */
	void* reallocf(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __STDLIB_H_
