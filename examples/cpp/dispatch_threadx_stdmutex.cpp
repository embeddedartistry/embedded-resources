#include <functional>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include <queue>
#include <mutex>
#include <string>
#include <condition_variable>
#include <threadx/tx_api.h>

#pragma mark - Definitions -

/// Definitions for dispatch event flags
#define DISPATCH_WAKE_EVT (0x1)
#define DISPATCH_EXIT_EVT (0x2)

/// Example thread priority and time slice
#define DISPATCH_Q_PRIORITY 15
#define DISPATCH_TIME_SLICE 5

/// Thread type
struct tx_thread_t
{
	TX_THREAD thread;
	std::string name;
	std::unique_ptr<uint8_t> stack;
};

/// This Bounce implementation is pulled from bounce.cpp
template<class T, class Method, Method m, class... Params>
static auto bounce(void* priv, Params... params)
	-> decltype(((*reinterpret_cast<T*>(priv)).*m)(params...))
{
	return ((*reinterpret_cast<T*>(priv)).*m)(params...);
}

/// Convenience macro to simplify bounce statement usage
#define BOUNCE(c, m) bounce<c, decltype(&c::m), &c::m>

#pragma mark - Dispatch Class -

class dispatch_queue
{
	typedef std::function<void(void)> fp_t;

  public:
	dispatch_queue(std::string name, size_t thread_cnt = 1, size_t thread_stack = 1024);
	~dispatch_queue();

	// dispatch and copy
	void dispatch(const fp_t& op);
	// dispatch and move
	void dispatch(fp_t&& op);

	// Deleted operations
	dispatch_queue(const dispatch_queue& rhs) = delete;
	dispatch_queue& operator=(const dispatch_queue& rhs) = delete;
	dispatch_queue(dispatch_queue&& rhs) = delete;
	dispatch_queue& operator=(dispatch_queue&& rhs) = delete;

  private:
	std::string name_;
	std::mutex mutex_;

	/// Vector of ThreadX Threads
	std::vector<tx_thread_t> threads_;

	/// TX event flags - like condition variable, used for waking queue threads
	TX_EVENT_FLAGS_GROUP notify_flags_;

	std::queue<fp_t> q_;
	bool quit_ = false;

	void dispatch_thread_handler(void);
};

#pragma mark - Implementation -

dispatch_queue::dispatch_queue(std::string name, size_t thread_cnt, size_t thread_stack_size) :
	name_{std::move(name)}, threads_(thread_cnt)
{
	// Create the event flags
	uint8_t status = tx_event_flags_create(&notify_flags_, "Dispatch Event Flags");
	assert(status == TX_SUCCESS && "Failed to create event flags!");

	// Dispatch thread setup
	for(size_t i = 0; i < threads_.size(); i++)
	{
		// allocate the thread stack
		threads_[i].stack.reset(new uint8_t[thread_stack_size]);

		// Define the name
		threads_[i].name = std::string("Dispatch Thread " + std::to_string(i));

		// Create and autostart the thread
		status = tx_thread_create(
			&threads_[i].thread, threads_[i].name.c_str(),
			reinterpret_cast<void (*)(ULONG)>(BOUNCE(dispatch_queue, dispatch_thread_handler)),
			reinterpret_cast<ULONG>(this), threads_[i].stack.get(), thread_stack_size,
			DISPATCH_Q_PRIORITY, DISPATCH_Q_PRIORITY, DISPATCH_TIME_SLICE, TX_AUTO_START);
		assert(status == TX_SUCCESS && "Failed to create thread!");
	}
}

// TODO: review
dispatch_queue::~dispatch_queue()
{
	uint8_t status;

	// Signal to dispatch threads that it's time to wrap up
	quit_ = true;

	// We will join each thread to confirm exiting
	for(size_t i = 0; i < threads_.size(); ++i)
	{
		UINT state;
		ULONG flags;
		do
		{
			// Signal wake - check exit flag
			tx_event_flags_set(&notify_flags_, DISPATCH_WAKE_EVT, TX_OR);

			// Wait until a thread signals exit. Timeout is acceptable.
			tx_event_flags_get(&notify_flags_, DISPATCH_EXIT_EVT, TX_OR_CLEAR, &flags, 10);

			// If it was not thread_[i], that is ok, but we will loop around
			// until threads_[i] has exited
			tx_thread_info_get(&threads_[i].thread, nullptr, &state, nullptr, nullptr, nullptr,
							   nullptr, nullptr, nullptr);
		} while(state != TX_COMPLETED);

		// threads_[i] has exited - let's delete it and move on to the next one
		status = tx_thread_delete(&threads_[i].thread);
		assert(status == TX_SUCCESS && "Failed to delete thread!");
		threads_[i].name.clear();
		threads_[i].stack.reset();
	}

	// Cleanup event flags and mutex
	tx_event_flags_delete(&notify_flags_);
}

void dispatch_queue::dispatch(const fp_t& op)
{
	std::lock_guard<std::mutex> lock(mutex_);

	q_.push(op);

	// Notifies threads that new work has been added to the queue
	tx_event_flags_set(&notify_flags_, DISPATCH_WAKE_EVT, TX_OR);
}

void dispatch_queue::dispatch(fp_t&& op)
{
	std::lock_guard<std::mutex> lock(mutex_);

	q_.push(std::move(op));

	// Notifies threads that new work has been added to the queue
	tx_event_flags_set(&notify_flags_, DISPATCH_WAKE_EVT, TX_OR);
}

void dispatch_queue::dispatch_thread_handler(void)
{
	ULONG flags;
	uint8_t status;

	std::unique_lock<std::mutex> lock(mutex_);

	do
	{
		// after wait, we own the lock
		if(!quit_ && q_.size())
		{
			auto op = std::move(q_.front());
			q_.pop();

			// unlock now that we're done messing with the queue
			lock.unlock();

			op();

			lock.lock();
		}
		else if(!quit_)
		{
			lock.unlock();

			// Wait for new work
			status = tx_event_flags_get(&notify_flags_, DISPATCH_WAKE_EVT, TX_OR_CLEAR, &flags,
										TX_WAIT_FOREVER);
			assert(status == TX_SUCCESS && "Failed to get event flags!");

			lock.lock();
		}
	} while(!quit_);

	// We were holding the mutex after we woke up
	lock.unlock();

	// Set a signal to indicate a thread exited
	status = tx_event_flags_set(&notify_flags_, DISPATCH_EXIT_EVT, TX_OR);
	assert(status == TX_SUCCESS && "Failed to set event flags!");
}
