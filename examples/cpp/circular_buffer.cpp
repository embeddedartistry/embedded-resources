#include <cstdio>

#include <memory>
#include <mutex>

/* DBJ 2019-02-03 --  standard ISO C++ is C++17, is this ok in the context of this project? */
/* DBJ 2019-02-03 --  re-use this whenever you need functions to work in presence of multiple threads
   see the usage bellow
*/
struct lock_unlock final {
 mutable std::mutex mux_;
 lock_unlock() noexcept { mux_.lock(); }
~lock_unlock() { mux_.unlock(); }
};

template <class T>
class circular_buffer {
public:
	/* DBJ 2019-02-03 -- Please consider, implementing copy semantics of this class
	std::unique_ptr can not be copied, thus compiler will refuse the default copy operator 
	of circular_buffer, since it's member 'buf_'  can not be copied
	circular_buffer move will work since std::unique_ptr can be moved
	*/
	
	/* DBJ 2019-02-03 -- Please consider
	constexpr inline std::uintmax_t MAX_CAPACITY = UINT16_MAX;
	UINT16_MAX is competely arbitrary 
	*/

	/* DBJ 2019-02-03 -- Please consider limiting the types this buffer can hold
	   for example:
	static_assert(	! std::is_pointer_v<T>, "\n\nNo pointers please\n" );
	static_assert(	! std::is_reference_v<T>, "\n\nNo references please\n" );
	*/

	/* DBJ 2019-02-03 -- Please consider simple  buffer non-destructive "reading" behaviour
	  in an  C++ std lib friendly manner, for example:
	   // just show the buffer
	   circular_buffer<int> circle(64) ;
	   for ( auto & el : circle ) { std::printf(" %4d ", el ); }
	*/
	using iterator = T * ;
	iterator begin () noexcept { return std::address_of( buf_[head_] ); }
	iterator end   () noexcept { return std::address_of( buf_[tail_] ); }
	// and so on ...
	
	/* DBJ 2019-02-03 -- put/get behaviour is perhaps more applicable to 'stack', not 'buffer'?
	   I have no enough context and requirements, thus I am not sure if this is ok comment
         */
	explicit circular_buffer(size_t size) :
		buf_(std::make_unique<T[]>(size)),
		max_size_(size)
	{

	}
	/* DBJ 2019-02-03 this requires T to be trivialy copyable */
	void put(T item) /* use noexcept wherever you can */
	{
		lock_unlock auto_lock_;

		buf_[head_] = item;

		if(full_)
		{
			tail_ = (tail_ + 1) % max_size_;
		}

		head_ = (head_ + 1) % max_size_;

		full_ = head_ == tail_;
	}

	// DBJ 2019-02-03 -- se the comment bellow
	// std::optional<T> get () const noexcept
	T get()  
	{
		lock_unlock auto_lock_;

		if(empty())
		{
			// DBJ 2019-02-03 
			// this requires T to have default constructor
			// also this requires logicaly "empty" state of T
			// which is not doable for every T
			// using C++17 std::optional<T> as return value
			// makes this kind of issues solvable
			return T();
		}

		//Read data and advance the tail (we now have a free space)
		auto val = buf_[tail_];
		full_ = false;
		tail_ = (tail_ + 1) % max_size_;

		return val;
	}

	void reset()
	{
		lock_unlock auto_lock_;
		head_ = tail_;
		full_ = false;
	}

	bool empty() const
	{
		//if head and tail are equal, we are empty
		return (!full_ && (head_ == tail_));
	}

	bool full() const
	{
		//If tail is ahead the head by 1, we are full
		return full_;
	}

	size_t capacity() const
	{
		return max_size_;
	}

	size_t size() const
	{
		size_t size = max_size_;

		if(!full_)
		{
			if(head_ >= tail_)
			{
				size = head_ - tail_;
			}
			else
			{
				size = max_size_ + head_ - tail_;
			}
		}
		return size;
	}

private:
	/* DBJ 2019-02-03 -- Please consider using "mutable", to ease the 
	implementation of the behavior of const instance of the circular_buffer
	*/
	// dbj removed -- mutable std::mutex mutex_;
	mutable std::unique_ptr<T[]> buf_;
	mutable size_t head_ = 0;
	mutable size_t tail_ = 0;
	const   size_t max_size_;
	mutable bool full_ = 0;
};

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
