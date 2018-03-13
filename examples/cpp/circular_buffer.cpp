#include <cstdio>

#include <memory>
#include <mutex>

/**
* Important Usage Note: This library reserves one spare entry for queue-full detection
* Otherwise, corner cases and detecting difference between full/empty is hard.
* You are not seeing an accidental off-by-one.
*/

template <class T>
class circular_buffer {
public:
	circular_buffer(size_t size) :
		buf_(std::unique_ptr<T[]>(new T[size])),
		size_(size)
	{
		//empty constructor
	}

	void put(T item)
	{
		std::lock_guard<std::mutex> lock(mutex_);

		buf_[head_] = item;
		head_ = (head_ + 1) % size_;

		if(head_ == tail_)
		{
			tail_ = (tail_ + 1) % size_;
		}
	}

	T get(void) const
	{
		std::lock_guard<std::mutex> lock(mutex_);

		if(empty())
		{
			return T();
		}

		//Read data and advance the tail (we now have a free space)
		auto val = buf_[tail_];
		tail_ = (tail_ + 1) % size_;

		return val;
	}

	void reset(void)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		head_ = tail_;
	}

	bool empty(void) const
	{
		//if head and tail are equal, we are empty
		return head_ == tail_;
	}

	bool full(void) const
	{
		//If tail is ahead the head by 1, we are full
		return ((head_ + 1) % size_) == tail_;
	}

	size_t size(void) const
	{
		return size_ - 1;
	}

private:
	std::mutex mutex_;
	std::unique_ptr<T[]> buf_;
	size_t head_ = 0;
	size_t tail_ = 0;
	size_t size_;
};

int main(void)
{
	circular_buffer<uint32_t> circle(10);
	printf("\n === CPP Circular buffer check ===\n");
	printf("Capacity: %zu\n", circle.size());
	uint32_t x = 1;
	printf("Put 1, val: %d\n", x);
	circle.put(x);
	x = circle.get();
	printf("Popped: %d\n", x);
	printf("Empty: %d\n", circle.empty());

	printf("Adding 9 values\n");
	for(uint32_t i = 0; i < circle.size(); i++)
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
