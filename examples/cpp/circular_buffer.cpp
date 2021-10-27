#include <cstdio>
#include <circular_buffer/circular_buffer.hpp>

int main(void)
{
	circular_buffer<uint32_t> circle(10);
	printf("\n === CPP Circular buffer check ===\n");
	printf("Size: %zu, Capacity: %zu\n", circle.size(), circle.capacity());

	uint32_t x = 1;
	printf("Put 1, val: %d\n", x);
	circle.put(x);

	x = circle.get();
	printf("Popped: %d\n", x);

	printf("Empty: %d\n", circle.empty());

	printf("Adding %zu values\n", circle.capacity() - 1);
	for(uint32_t i = 0; i < circle.capacity() - 1; i++)
	{
		circle.put(i);
	}

	circle.reset();

	printf("Full: %d\n", circle.full());

	printf("Adding %zu values\n", circle.capacity());
	for(uint32_t i = 0; i < circle.capacity(); i++)
	{
		circle.put(i);
	}

	printf("Full: %d\n", circle.full());

	printf("Reading back values: ");
	while(!circle.empty())
	{
		printf("%u ", circle.get());
	}
	printf("\n");

	printf("Adding 15 values\n");
	for(uint32_t i = 0; i < circle.size() + 5; i++)
	{
		circle.put(i);
	}

	printf("Full: %d\n", circle.full());

	printf("Reading back values: ");
	while(!circle.empty())
	{
		printf("%u ", circle.get());
	}
	printf("\n");

	printf("Empty: %d\n", circle.empty());
	printf("Full: %d\n", circle.full());

	return 0;
}
