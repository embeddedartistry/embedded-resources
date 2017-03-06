#ifndef __STRING_H_
#define __STRING_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <stdint.h>
#include <stddef.h>

#pragma mark - memory -

int memcmp(const void * s1, const void * s2, size_t n);
void *memset(void * dest, int c, size_t n);
void *memcpy(void * __restrict dest, const void * __restrict src, size_t n);
void *memmove(void * dest, const void * src, size_t n);
void *memchr(const void *s, int c, size_t n);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif /* __STRING_H_ */
