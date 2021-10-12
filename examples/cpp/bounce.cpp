#include <cstdio>

// Type definitions for some callback function prototypes
typedef void(cb_func1)(void* priv, int input);
typedef void(cb_func2)(void* priv);

/**
 * This is a basic class which has a public function interface that we
 * will pass into a callback system.
 *
 * You can use a seed to ensure that your intended object is the one being called.
 */
class FooCB
{
  public:
	FooCB(void) : seed_(0)
	{
	}
	FooCB(int t) : seed_(t)
	{
	}

	void callback(int input) const
	{
		printf("FooCB callback, seed+input: %d\n", seed_ + input);
	}

	void callback2(void) const
	{
		printf("FooCB callback2, seed: %d\n", seed_);
	}

  private:
	int seed_;
};

/**
 * Below are two functions that we can use to demonstrate use of the bounce
 * function. The callbacks are setup in the C-style manner, demonstrating two common forms:
 * 	-callback_test takes a function pointer, private data, and input value
 *	-callback_test_no_input leaves off the input value, providing only private data
 */
static void callback_test(cb_func1 cb_func, void* priv, int input)
{
	if(cb_func)
	{
		cb_func(priv, input);
	}
}

static void callback_test_no_input(cb_func2 cb_func, void* priv)
{
	if(cb_func)
	{
		cb_func(priv);
	}
}

/**
 * This is our "bounce" function template.
 * This function is intended to be passed to a C-style callback system which takes
 * a function pointer and private data pointer as input values. Using C-style callbacks
 * is problematic with object member functions, as you need the object's pointer.
 *
 * This bounce function requires use of the private data pointer for the object's
 * instance pointer. Any other provided arguments are forwarded to the object's member
 * function, and the return value is passed to the caller.
 */
template<class T, class Method, Method m, class... Params>
static auto bounce(void* priv, Params... params)
	-> decltype(((*reinterpret_cast<T*>(priv)).*m)(params...))
{
	return ((*reinterpret_cast<T*>(priv)).*m)(params...);
}

/**
 * Convenience macro to simplify bounce statement usage
 */
#define BOUNCE(c, m) bounce<c, decltype(&c::m), &c::m>

int main(void)
{
	// We want the callback from this specific instance to be used
	FooCB my_foo(2038);

	/*
	 * As you can see, this full bounce declaration is pretty verbose
	 *
	 * First argument is the templated bounce function, which specifies:
	 *	- The class we are templating off of
	 *	- The decltype of our callback function
	 *	- The member function pointer we want to call
	 *
	 * Next we specify the pointer to the specific object we want to reference
	 * And finally, we have the input value which will be forwarded to the callback
	 */
	callback_test(&bounce<FooCB, decltype(&FooCB::callback), &FooCB::callback>, &my_foo, 5000);

	/*
	 * Using the bounce macro, we can simplify our typing significantly. Now we
	 * only need to specify the class and member function inside the macro.
	 */
	callback_test(&BOUNCE(FooCB, callback), &my_foo, 1001);

	/**
	 * The only requirements are that you need to register a function pointer
	 * and private data pointer! No other input is needed.
	 */
	callback_test_no_input(&BOUNCE(FooCB, callback2), &my_foo);

	return 0;
}
