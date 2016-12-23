#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define offset_of(t, m) \
	((size_t)&(((t*)0)->m))

struct test {
	int a;
	char b;
	uint32_t c;
};

struct test2 {
	int a;
	char b;
	uint32_t c;
} __attribute__((packed));

int main(void)
{
	printf("offset_of a: %zu / b: %zu / c: %zu\n",
		offset_of(struct test, a),
		offset_of(struct test, b),
		offset_of(struct test, c)
	);

	printf("Packed: offset_of a: %zu / b: %zu / c: %zu\n",
		offset_of(struct test2, a),
		offset_of(struct test2, b),
		offset_of(struct test2, c)
	);
}
