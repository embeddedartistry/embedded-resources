//#define USE_EXAMPLE_1 // Legacy code example
//#define USE_EXAMPLE_2 // Intermediate refactoring example
#define USE_EXAMPLE_3 // Full refactoring

#ifdef USE_EXAMPLE_1
#include <stdio.h>

int data_0 = 1;
int data_1 = 2;
int data_2 = 3;
int data_3 = 4;

int func0(void)
{
	return data_0 + data_1;
}

int func1(void)
{
	return data_2 + data_3;
}

int main(void)
{
	int a = func0();
	int b = func1();
	int calculated = a + b;
	printf("Calculated value: %d\n", calculated);

	return 0;
}
#elif defined(USE_EXAMPLE_2)
#include <stdio.h>

typedef struct {
	int data_0;
	int data_1;
	int data_2;
	int data_3;
} global_data_t;

global_data_t instance_1 = {
	.data_0 = 1,
	.data_1 = 2,
	.data_2 = 3,
	.data_3 = 4
};

global_data_t* g_data_ptr;
#define data_0 (g_data_ptr->data_0)
#define data_1 (g_data_ptr->data_1)
#define data_2 (g_data_ptr->data_2)
#define data_3 (g_data_ptr->data_3)

int func0(void)
{
	return data_0 + data_1;
}

int func1(void)
{
	return data_2 + data_3;
}

int main(void)
{
	g_data_ptr = &instance_1;

	int a = func0();
	int b = func1();
	int calculated = a + b;
	printf("Calculated value: %d\n", calculated);

	return 0;
}
#elif defined(USE_EXAMPLE_3)
#include <stdio.h>

typedef struct {
	int data_0;
	int data_1;
	int data_2;
	int data_3;
} global_data_t;

global_data_t instance_1 = {
	.data_0 = 1,
	.data_1 = 2,
	.data_2 = 3,
	.data_3 = 4
};

global_data_t instance_2 = {
	.data_0 = 5,
	.data_1 = 6,
	.data_2 = 7,
	.data_3 = 8
};

global_data_t* g_data_ptr;
#define data_0 (g_data_ptr->data_0)
#define data_1 (g_data_ptr->data_1)
#define data_2 (g_data_ptr->data_2)
#define data_3 (g_data_ptr->data_3)

int func0(global_data_t* g_data_ptr)
{
	return data_0 + data_1;
}

int func1(global_data_t* g_data_ptr)
{
	return data_2 + data_3;
}

int main(void)
{
	int a = func0(&instance_1);
	int b = func1(&instance_1);
	int calculated = a + b;
	printf("Calculated value for instance_1: %d\n", calculated);

	a = func0(&instance_2);
	b = func1(&instance_2);
	calculated = a + b;
	printf("Calculated value for instance_2: %d\n", calculated);

	return 0;
}
#else
#error Please enable an example using one of the macros shown above.
#endif
