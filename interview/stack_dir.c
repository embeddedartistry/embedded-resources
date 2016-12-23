#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// true if grows up, false if grows down
static bool stack_dir1()
{
	int a;
	int b;

	return ((uintptr_t)&b) > ((uintptr_t)&a);
}

int main(void)
{
	printf("Approach 1: The stack grows %s\n", stack_dir1() ? "up" : "down");
}
