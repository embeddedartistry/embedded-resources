#ifndef __STDDEF_H_
#define __STDDEF_H_

#include <_types/_ptrdiff_t.h>
#include <_types/_size_t.h>

#pragma mark - NULL -

#ifdef __cplusplus
	#define NULL 0L
#else
	// not C++
	#define NULL ((void*)0)
#endif

#pragma mark - offsetof -

#if __GNUC__ > 3
	#define offsetof(type, member) __builtin_offsetof(type, member)
#else
	#define offsetof(type, member) ((size_t)((char*)&(((type*)0)->member) - (char*)0))
#endif

#pragma mark - max_align_t -

#if(defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
	(defined(__cplusplus) && __cplusplus >= 201103L)
typedef long double max_align_t;
#endif

#endif //__STDDEF_H_
