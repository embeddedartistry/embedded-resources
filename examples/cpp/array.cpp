#include <iostream>
#include <array>

void carr_func(int * arr, size_t size)
{
	std::cout << "carr_func - arr: " << arr << std::endl;
}

int main(void)
{
	//Declares an array of 10 ints. Size is always required
	std::array<int, 10> a1;

	//Declare and initialize
	std::array<int, 5> a2 = {-1, 1, 3, 2, 0};

	std::array<int, 0> a_empty;

	//The std::array container keeps track of size
	std::cout << "a1.size(): " << a1.size() << std::endl;
	std::cout << "a2.size(): " << a2.size() << std::endl;
	//Note: For std::array, size() == max_size()
	std::cout << "a2.max_size(): " << a2.max_size() << std::endl;

	/*
	* Is the array empty?
	* Only if you have an array of size 0. Unitialized arrays still
	* have a size and are never empty!
	*/
	std::cout << "a1.empty(): " << a1.empty() << std::endl;
	std::cout << "a2.empty(): " << a2.empty() << std::endl;
	std::cout << "a_empty.empty(): " << a_empty.empty() << std::endl;

	//Element Access options
	std::cout << "a2.front(): " << a2.front() << std::endl;
	std::cout << "a2.back(): " << a2.back() << std::endl;
	std::cout << "a2[0]: " << a2[0] << std::endl;
	std::cout << "a2.at(4): " << a2.at(4) << std::endl;

	// Bounds checking will generate exceptions. Try:
	//auto b = a2.at(10);

	//However, operator [] is not bounds checked!
	//This may or may not seg fault
	//std::cout << "a2[7]: " << a2[7] << std::endl;

	//Making a new array via copy
	auto a3 = a2;

	//Assigning values works as expected
	a3[0] = -2;

	/*
	* If you need to interface with legacy code or libraries requiring
	* a C-style array interface, you can get to the underlying array data ptr
	*/

	//Error:
	//carr_func(a2, a2.size());

	//OK:
	carr_func(a2.data(), a2.size());

	// You can use convenient ranged for loops!
	std::cout << std::endl << "a3: " << std::endl;
	for (const auto & t : a3)
	{
		std::cout << t << " ";
	}
	std::cout << std::endl;

	//You can use container operations
	std::sort(a3.begin(), a3.end());

	// You can use convenient ranged for loops!
	std::cout << std::endl << "a3 std::sort: " << std::endl;
	for (const auto & t : a3)
	{
		std::cout << t << " ";
	}
	std::cout << std::endl;

	//Assign a2 to a3's values:
	a2 = a3;

	// But you can only use the '=' operator on arrays of equivalent size.
	//Error:
	//a1 = a2; //<[...],10> vs <[...],5>! invalid

	std::cout << std::endl << "a2, post-assignment: " << std::endl;
	for (const auto & t : a2)
	{
		std::cout << t << " ";
	}
	std::cout << std::endl;

	return 0;
}
