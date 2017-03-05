#ifndef __LL_H_
#define __LL_H_

#include <stdlib.h> //size_t, NULL

/**
* Define offsetof and container_of if we don't have them already
*/
#ifndef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE, MEMBER) __compiler_offsetof(TYPE, MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif
#endif //offsetof

#ifndef container_of
#define container_of(ptr, type, member)	({			\
	const __typeof__( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif //container_of

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*
 * This is a doubly linked list structure.
 * This structure should be embedded in a container structure that you want to list.
 */
typedef struct ll_head {
	struct ll_head *next;	/**< Next pointer */
	struct ll_head *prev;	/**< Previous pointer */
} ll_t;

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_first_entry(head, type, member) list_entry((head)->next, type, member)

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

#define list_for_each_entry(pos, head, member) \
	for (pos = list_entry((head)->next, __typeof__(*pos), member);	\
		&pos->member != (head);					\
		pos = list_entry(pos->member.next, __typeof__(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, __typeof__(*pos), member),	\
		n = list_entry(pos->member.next, __typeof__(*pos), member);	\
		&pos->member != (head);					\
		pos = n, n = list_entry(n->member.next, __typeof__(*n), member))

#pragma mark - Init -

#define ll_head_INIT(name)	{ &(name), &(name) }
#define LIST_INIT(name)		struct ll_head name = ll_head_INIT(name)

#pragma mark - Add -

static inline void list_add_(struct ll_head *n, struct ll_head *prev,
		struct ll_head *next)
{
	next->prev = n;
	n->next = next;
	n->prev = prev;
	prev->next = n;
}

static inline void list_add(struct ll_head *n, struct ll_head *head)
{
	list_add_(n, head, head->next);
}

static inline void list_add_tail(struct ll_head *n, struct ll_head *head)
{
	list_add_(n, head->prev, head);
}

#pragma mark - Delete -

static inline void list_del_(struct ll_head *prev, struct ll_head *next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct ll_head *entry)
{
	list_del_(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __LL_H_
