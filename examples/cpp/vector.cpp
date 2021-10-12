#include <iostream>
#include <vector>

void carr_func(int* vec)
{
	std::cout << "carr_func - vec: " << vec << std::endl;
}

int main(void)
{
	std::vector<int> v1 = {-1, 3, 5, -8, 0}; // initialize with list
	std::vector<int> v2; // don't initialize
	auto v3(v1); // initialize v3 via copy

	/**
	 * Managing std::vector capacity
	 */

	// Unlike std::array, std::vector has a more sensible empty() function
	// v2 is currently empty
	std::cout << "v1.empty(): " << v1.empty() << std::endl;
	std::cout << "v2.empty(): " << v2.empty() << std::endl;

	// size() tells you the number of elements
	std::cout << "v1.size(): " << v1.size() << std::endl;
	std::cout << "v2.size(): " << v2.size() << std::endl;

	// max_size() is huuuuuuuuuge for my host machine
	std::cout << "v1.max_size(): " << v1.max_size() << std::endl;
	std::cout << "v2.max_size(): " << v2.max_size() << std::endl;

	// Capacity tells you how many elements can be stored in the currently allocated memory
	std::cout << "v1.capacity(): " << v1.capacity() << std::endl;
	std::cout << "v2.capacity(): " << v2.capacity() << std::endl;

	v2.reserve(10);
	std::cout << "v2.capacity() after reserve(10): " << v2.capacity() << std::endl;
	std::cout << "v2.size(): " << v2.size() << std::endl;

	// If you have reserved space greater than your current needs, you can shrink the buffer
	v2.shrink_to_fit();
	std::cout << "v2.capacity() after shrink_to_fit(): " << v2.capacity() << std::endl;

	/**
	 * Accessing std::vector elements
	 */
	std::cout << "v1.front(): " << v1.front() << std::endl;
	std::cout << "v1.back(): " << v1.back() << std::endl;
	std::cout << "v1[0]: " << v1[0] << std::endl;
	std::cout << "v1.at(4): " << v1.at(4) << std::endl;

	// Bounds checking will generate exceptions. Try:
	// auto b = v2.at(10);

	// However, operator [] is not bounds checked!
	// This may or may not seg fault
	// std::cout << "v2[6]: " << v2[6] << std::endl;

	/*
	 * If you need to interface with legacy code or libraries requiring
	 * a C-style array interface, you can get to the underlying array data ptr
	 */

	// Error:
	// carr_func(v1);

	// OK:
	carr_func(v1.data());

	/**
	 * Playing around with vectors
	 */
	v2 = v1; // copy

	std::cout << "v2.size() after copy: " << v2.size() << std::endl;
	v2.clear();
	std::cout << "v2.size() after clear: " << v2.size() << std::endl;
	std::cout << "v2.capacity(): " << v2.capacity() << std::endl;

	v2.insert(v2.begin(), -1); // insert an element - you need an iterator
	v2.emplace(v2.end(), int(1000)); // construct and place an element at the iterator

	int x = 10;
	v2.push_back(x); // adds element to end
	v2.emplace_back(10); // constructs an element in place at the end

	std::cout << std::endl << "v2: " << std::endl;
	for(const auto& t: v2)
	{
		std::cout << t << " ";
	}
	std::cout << std::endl;

	v2.resize(7); // resize to 7. The new elements will be 0-initialized
	v2.resize(10, -1); // resize to 10. New elements initialized with -1

	v2.pop_back(); // removes last element
	v2.erase(v2.begin()); // removes first element

	std::cout << std::endl << "v2 resized: " << std::endl;
	for(const auto& t: v2)
	{
		std::cout << t << " ";
	}
	std::cout << std::endl;

	v2.resize(4); // shrink and strip off extra elements

	// Container operations work
	std::sort(v2.begin(), v2.end());

	std::cout << std::endl << "v2 shrunk & sorted: " << std::endl;
	for(const auto& t: v2)
	{
		std::cout << t << " ";
	}
	std::cout << std::endl;

	std::cout << "std::vector size: " << sizeof(std::vector<char>) << std::endl;

	return 0;
}
