#include <stdio.h>

void foo(void)
{
	int a = 5;
	int b;
}

void bar(void)
{
	int x;
	printf("%d\n", x++);
}

int main(void)
{
	foo();
	bar();
	bar();

	return 0;
}
