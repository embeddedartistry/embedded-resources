#include <iostream>
#include <string>

int main(void)
{
	// declaring a string variable
	std::string my_str;

	// initialize with a C-string
	std::string name("Phillip");

	// initialize via copy of std::string
	std::string name2(name);

	// initialize as a substring of a std::string
	std::string lip(name, 4);

	// fill the string with a char. note the single quotes
	std::string filled(16, 'A');

	// Let's print the values out to see
	std::cout << "name: " << name << std::endl;
	std::cout << "name2: " << name2 << std::endl;
	std::cout << "lip: " << lip << std::endl;
	std::cout << "filled: " << filled << std::endl;
	std::cout << "my_str: " << my_str << std::endl;

	// c-string assignment works
	my_str = "Phillip";
	std::cout << "my_str (post C-str): " << my_str << std::endl;

	// Copy assignment
	my_str = filled;
	std::cout << "my_str (post copy): " << my_str << std::endl;

	// Move assignment
	my_str = std::move(name2);
	std::cout << "my_str (post move): " << my_str << std::endl;
	std::cout << "name2 (post move): " << name2 << std::endl;

	// We can compare std::string objects for equality
	if(my_str != name2)
	{
		std::cout << "After move, my_str and name2 no longer match!" << std::endl;
	}

	name2 = std::string("Phillip"); // build a temp std::string obj for assignment
	if(my_str == name2)
	{
		std::cout << "Now my_str and name2 match!" << std::endl;
	}

	// We can compare std::string objects for equality, as long as one is a std::string
	if(my_str == "Phillip")
	{
		std::cout << "my_str and \"Phillip\" match!" << std::endl;
	}

	// Concatenation is also very simple!
	my_str = lip + name2;
	my_str += "lip"; // C-string cat works too
	std::cout << "my_str (post concat): " << my_str << std::endl;

	// You can still use the indexing operator: []
	std::cout << "name2[0]: " << name2[0] << ", name2[3]: " << name2[3] << std::endl;

	// std::string contains lots of builtin helper functions
	my_str = "Phillip";
	std::cout << "my_str.size(): " << my_str.size() << std::endl;
	std::cout << "my_str.length(): " << my_str.length() << std::endl;
	std::cout << "my_str.capacity(): " << my_str.capacity() << std::endl;
	std::cout << "my_str.substr(2, 4): " << my_str.substr(2, 4) << std::endl;
	std::cout << "my_str.compare(lip): " << my_str.compare(lip) << std::endl;
	std::cout << "my_str.find(lip): " << my_str.find(lip) << std::endl;
	std::cout << "my_str.insert(3, lip): " << my_str.insert(5, lip) << std::endl;

	// clear a string
	my_str.clear();
	std::cout << "my_str (post-clear): " << my_str << std::endl;
	std::cout << "my_str.empty(): " << my_str.empty() << std::endl;

	return 0;
}
