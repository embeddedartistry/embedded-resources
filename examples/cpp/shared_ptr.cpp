#include <memory>
#include <iostream>
#include <string>

class SharedThing : public std::enable_shared_from_this<SharedThing> {
public:
	std::string name(void) {return name_;};

#if 0
	//Do not do this! creates multiple control blocks
	std::shared_ptr<SharedThing> getSharedThing() {
		return this;
	}
#endif

	//Lets us vend shared pointers to *this
	//Requires a shared_ptr to be created before use
	std::shared_ptr<SharedThing> getSharedThing() {
		return shared_from_this();
	}

	// factory function that perfect-forwards args
	// to a private ctor - we use this to create objects
	template<typename ... T>
	static std::shared_ptr<SharedThing> create(T&& ... t) {
		struct EnableMakeShared : public SharedThing {
			EnableMakeShared(Arg&&... arg) : SharedThing(std::forward<Arg>(arg)...) {}
		};
		return std::make_shared<EnableMakeShared>(std::forward<Arg>(arg)...);
	}

private:
	std::string name_;

	//We make constructor private - shared_from_this() breaks if a shared
	//ptr has never been created before.  (It looks for the control block)
	SharedThing() : name_("Nameless Thing") {}
	SharedThing(std::string name) : name_(name) {}
};

int main(void)
{
	//Creating a shared ptr.
	std::shared_ptr<uint32_t> x(new uint32_t(0x1234abcd));

	//Note how much cleaner the make_shared alternative is.
	auto y(std::make_shared<uint32_t>(0xdeadbeef));

	auto z(x); //z is now a shared ptr to x via copy

	//Dereference ops work as expected
	std::cout << "x: " << std::hex << *x << std::endl;
	std::cout << "y: " << std::hex << *y << std::endl;
	std::cout << "z: " << std::hex << *z << std::endl;

	std::weak_ptr<uint32_t> wptr(y); //wptr is a weak pointer to y

	//We're going to use wptr to get to our shared_ptr, and prove it
	std::shared_ptr<uint32_t> sptr(wptr);
	//Get also works for shared_ptrs - same rules apply, you don't own it!
	std::cout << "y@" << std::hex << y.get() << std::endl;
	std::cout << "sptr@" << std::hex << sptr.get() << std::endl;
	sptr.reset(); //refcnt now 1
	sptr = wptr.lock(); //another way to convert weak->shared - refcnt now 2
	sptr = nullptr; //refcnt now 1

	if(!wptr.expired()) {
		std::cout << "wptr is still valid" << std::endl;
	} else {
		std::cout << "wptr is expired" << std::endl;
	}

	y = nullptr; //refcnt is 0, delete y

	if(!wptr.expired()) {
		std::cout << "wptr is still valid" << std::endl;
	} else {
		std::cout << "wptr is expired" << std::endl;
	}

	//lock() will now vend nullptr since we don't have a valid object
	sptr = wptr.lock();
	std::cout << "sptr@" << sptr.get() << std::endl;

	//We use the factory and get a valid shared ptr out
	auto SharedThing = SharedThing::create("Thing 1");
	std::cout << "Created SharedThing: " << SharedThing->name() << std::endl;

	//Now we can actually get instances of Thing 1
	auto t = SharedThing->getSharedThing();
	std::cout << "t points to: " << t->name() << std::endl;

	//Yay templates! different constructors will work
	auto SharedThing2 = SharedThing::create();
	std::cout << "Thanks to perfect-forwarding, we also created: " <<
		SharedThing2->name() << std::endl;

	return 0;
}
