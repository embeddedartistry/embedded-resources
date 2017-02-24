#include <memory>
#include <cstdio>
#include <cstdint>
#include <string>
#include "../c/memory.h"

static void aligned_free_wrapper(void * ptr)
{
	printf("Calling aligned_free on ptr: %p\n", ptr);
	aligned_free(ptr);
}

/**
* Here we create aliases for our aligned pointer types.
* We are specifying that our alias has a fixed value: the deleter function type
* We use this for unique_ptr as it requires the type of the deleter in the declaration
*/
template<class T> using unique_ptr_aligned = std::unique_ptr<T, decltype(&aligned_free)>;

template<class T>
unique_ptr_aligned<T> aligned_ptr(size_t align, size_t size)
{
	return unique_ptr_aligned<T>(static_cast<T*>(aligned_malloc(align, size)), &aligned_free_wrapper);
}

int main(void)
{
	/**
	* Using our unique_ptr_aligned type, we declare an aligned uint8_t unique ptr.
	* I use my wrapper function as the deleter to print values as things are getting freed
	* Note that rather than calling new(uint8_t), we call aligned_malloc to get our aligned memory
	*/
	unique_ptr_aligned<uint8_t[]> x(static_cast<uint8_t*>(aligned_malloc(8, 1024)), &aligned_free_wrapper);
	auto y = aligned_ptr<uint8_t>(32, 100);

	/**
	* Why don't we need a shared pointer to also have an alias?
	* The deleter is not part of the type, simply the constructor
	*/
	std::shared_ptr<uint8_t> z(static_cast<uint8_t*>(aligned_malloc(32, 128)), &aligned_free_wrapper);

	printf("x (unique) has alignment 8: %p\n", x.get());
	printf("y (unique) has alignment 32: %p\n", y.get());
	printf("z (shared) has alignment 32: %p\n", z.get());

	/**
	* This will cause the deleter to be called (aligned_free)
	*/
	printf("Freeing y pointer by reset()\n");
	y.reset();

	/*
	* Since these pointers are allocated on the stack, they are bound by RAII
	* Once we leave main() the pointers will go out of scope and the deleter we specified
	* will be called automatically.
	*/
	printf("Leaving main. X and Z will be automatically destructed\n");

	return 0;
}
