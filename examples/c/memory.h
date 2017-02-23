#ifndef __MEMORY_H_
#define __MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

void * aligned_malloc(size_t align, size_t size);
void aligned_free(void * ptr);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __MEMORY_H_
