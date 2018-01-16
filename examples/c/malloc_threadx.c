#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <tx_api.h>

#pragma mark - Definitions -

/**
* These are example addresses for the purposes of compiling.
* The addresses used should be externally defined in another file for your system
*/
#define HEAP_START 0x10008000
#define HEAP_END 0x1000FFFF

/*
* In this example, we treat malloc as a function pointer
* we initialize the value as init_malloc.
* The first function to call malloc() will take the hit on initializing the pool
* Afterwards, all calls to malloc() pass through to do_malloc().
*/
typedef void *(* malloc_ptr_t)(size_t);

/*
* In this example, I am using the compiler's builtin atomic compare and swap
* Routine. This will provide atomic access to swapping the malloc pointer,
* and only one function will initialize the memory pool.
*/
#define atomic_compare_and_swap __sync_val_compare_and_swap

#pragma mark - Prototypes -

static void * init_malloc(size_t size);
static void * do_malloc(size_t size);

#pragma mark - Declarations -

// ThreadX internal memory pool stucture
static TX_BYTE_POOL malloc_pool_ = {0};

/*
* Flag that is used in do_malloc() to cause competing threads to wait until
* initialization is completed before allocating memory.
*/
volatile static bool initialized_ = false;

/**
* Note well that I have redefined 'malloc' to be a volatile function pointer.
* This will cause compiler errors if you include the compiler's stdlib.h.
* You will need to redefine another header for malloc/free/etc. (e.g. memory.h)
* or write your own string.h/stdlib.h headers that will be included over the compiler's version
*
* This also means you need to compile with -fno-builtin and link with -nodefaultlibs
*/
volatile malloc_ptr_t malloc = &init_malloc;

#pragma mark - Private Functions -

/*
 * init_malloc must be called before memory allocation calls are made
 * This sets up a byte pool for the heap using the defined HEAP_START and HEAP_END macros
 * Size is passed to do_malloc and allocated to the caller
 */

void * init_malloc(size_t size)
{
	assert(size > 0);

	uintptr_t heap_start = (uintptr_t)HEAP_START;
	uintptr_t heap_size = (uintptr_t)HEAP_END - heap_start;

	/**
	* When we enter into init_malloc, we check the current value of the malloc pointer
	* If it's still init_malloc, we swap the value to do_malloc and return true.
	* If the value is do_malloc, another thread beat us to the punch and we fall
	* through to do_malloc(), skipping initialization.
	*/
	if(atomic_compare_and_swap(&malloc, &init_malloc, &do_malloc))
	{
		uint8_t r;

		/**
		* This is ThreadX's API to create a byte pool using a memory block.
		* We are essentially just wrapping ThreadX APIs into a simpler form
		*/
		r = tx_byte_pool_create(&malloc_pool_, "Heap Memory Pool",
				(void *)heap_start,
				heap_size);
		assert(r == TX_SUCCESS);

		//Signal to any threads waiting on do_malloc that we are done
		initialized_ = true;
	}

	/*
	* Remember - two situations happen here:
	* 	1) malloc initialized on the first call, and then passed through to normal malloc
	*	2) two threads raced to init. One initializes, and the other falls through to malloc
	*/
	return do_malloc(size);
}

void * do_malloc(size_t size)
{
	void * ptr = NULL;

	/**
	* Since multiple threads could be racing to malloc, if we lost the race
	* we need to make sure the ThreadX pool has been created before we
	* try to allocate memory, or there will be an error
	*/
	while(!initialized_)
	{
		tx_thread_sleep(1);
	}

	if(size > 0)
	{
		// We simply wrap the threadX call into a standard form
		uint8_t r = tx_byte_allocate(&malloc_pool_, &ptr, size, TX_WAIT_FOREVER);

		//I add the string to provide a more helpful error output.  It's value is always true.
		assert(r == TX_SUCCESS && "malloc failed");
	} //else NULL if there was an error

	return ptr;
}

void free(void * ptr)
{
	//free should NEVER be called before malloc is init'd
	assert(initialized_);

	if(ptr) {
		//We simply wrap the threadX call into a standard form
		uint8_t r = tx_byte_release(ptr);
		ptr = NULL;
		assert(r == TX_SUCCESS);
	}
}
