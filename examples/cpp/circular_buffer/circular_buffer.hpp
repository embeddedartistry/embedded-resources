#ifndef CIRCULAR_BUFFER_HPP_
#define CIRCULAR_BUFFER_HPP_

#include <array>
#include <mutex>

template<class T, size_t TElemCount>
class circular_buffer
{
  public:
	explicit circular_buffer() = default;

	void put(T item) noexcept
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);

		buf_[head_] = item;

		if(full_)
		{
			tail_ = (tail_ + 1) % TElemCount;
		}

		head_ = (head_ + 1) % TElemCount;

		full_ = head_ == tail_;
	}

	T get() noexcept
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);

		if(empty())
		{
			return T();
		}

		// Read data and advance the tail (we now have a free space)
		auto val = buf_[tail_];
		full_ = false;
		tail_ = (tail_ + 1) % TElemCount;

		return val;
	}

	void reset() noexcept
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		head_ = tail_;
		full_ = false;
	}

	bool empty() const noexcept
	{
		// Can have a race condition in a multi-threaded application
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		// if head and tail are equal, we are empty
		return (!full_ && (head_ == tail_));
	}

	bool full() const noexcept
	{
		// If tail is ahead the head by 1, we are full
		return full_;
	}

	size_t capacity() const noexcept
	{
		return TElemCount;
	}

	size_t size() const noexcept
	{
		// A lock is needed in size ot prevent a race condition, because head_, tail_, and full_
		// can be updated between executing lines within this function in a multi-threaded
		// application
		std::lock_guard<std::recursive_mutex> lock(mutex_);

		size_t size = TElemCount;

		if(!full_)
		{
			if(head_ >= tail_)
			{
				size = head_ - tail_;
			}
			else
			{
				size = TElemCount + head_ - tail_;
			}
		}

		return size;
	}

  private:
	mutable std::recursive_mutex mutex_;
	mutable std::array<T, TElemCount> buf_;
	mutable size_t head_ = 0;
	mutable size_t tail_ = 0;
	mutable bool full_ = 0;
};

#endif
