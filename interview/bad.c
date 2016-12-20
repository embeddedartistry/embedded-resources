#include <stdio.h>

void foo(void)
{
	int a = 5;
	int b;
}

void bar(void)
{
	int a;
	printf("%d\n", a++);
}

int main(void)
{
	foo();
	bar();
	bar();

	return 0;
}
