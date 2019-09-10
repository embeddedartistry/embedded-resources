#include <thread>
#include <functional>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <queue>
#include <mutex>
#include <string>
#include <condition_variable>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>

#pragma mark - Definitions -

/// Definitions for dispatch event flags
#define DISPATCH_WAKE_EVT	(0x1)
#define DISPATCH_EXIT_EVT	(0x2)

/// Example thread priority and time slice
#define DISPATCH_Q_PRIORITY 15

/// Thread type
struct freertos_thread_t {
	TaskHandle_t thread;
	std::string name;
};

/// This Bounce implementation is pulled from bounce.cpp
template<class T, class Method, Method m, class ...Params>
static auto bounce(void *priv, Params... params) ->
		decltype(((*reinterpret_cast<T *>(priv)).*m)(params...))
{
	return ((*reinterpret_cast<T *>(priv)).*m)(params...);
}

/// Convenience macro to simplify bounce statement usage
#define BOUNCE(c,m) bounce<c, decltype(&c::m), &c::m>

#pragma mark - Dispatch Class -

class dispatch_queue {
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

	// FreeRTOS uses semaphore handles for mutexes
	SemaphoreHandle_t mutex_;

	/// Vector of FreeRTOS Threads
	std::vector<freertos_thread_t> threads_;

	/// FreeRTOS event flags - like condition variable, used for waking queue threads
	EventGroupHandle_t notify_flags_;

	std::queue<fp_t> q_;
	bool quit_ = false;

	void dispatch_thread_handler(void);
};

#pragma mark - Implementation -

dispatch_queue::dispatch_queue(std::string name, size_t thread_cnt, size_t thread_stack_size) :
	name_{std::move(name)}, threads_(thread_cnt)
{
	// Create the Mutex
	mutex_ = xSemaphoreCreateRecursiveMutex();
	assert(mutex_ != NULL && "Failed to create mutex!");

	// Create the event flags
	notify_flags_ = xEventGroupCreate();
	assert(notify_flags_ != NULL && "Failed to create event group!");

	// Dispatch thread setup
	for(size_t i = 0; i < threads_.size(); i++)
	{
		// Define the name
		threads_[i].name = std::string("Dispatch Thread " + std::to_string(i));

		// Create the thread
		BaseType_t status = xTaskCreate(reinterpret_cast<void(*)(void*)>(
								BOUNCE(dispatch_queue, dispatch_thread_handler)),
								threads_[i].name.c_str(),
								thread_stack_size,
								reinterpret_cast<void*>(this),
								DISPATCH_Q_PRIORITY,
								&threads_[i].thread);
		assert(status == pdPASS && "Failed to create thread!");
	}
}

dispatch_queue::~dispatch_queue()
{
	// Signal to dispatch threads that it's time to wrap up
	quit_ = true;

	// We will join each thread to confirm exiting
	for (size_t i = 0; i < threads_.size(); ++i) {
		eTaskState state;

		do {
			// Signal wake - check exit flag
			xEventGroupSetBits(notify_flags_, DISPATCH_WAKE_EVT);

			// Wait until a thread signals exit. Timeout is acceptable.
			xEventGroupWaitBits(notify_flags_, DISPATCH_EXIT_EVT, pdTRUE, pdFALSE, 10);

			// If it was not thread_[i], that is ok, but we will loop around
			// until threads_[i] has exited
			state = eTaskGetState(threads_[i].thread);
		} while (state != eDeleted);

		threads_[i].name.clear();
	}

	// Cleanup event flags and mutex
	vEventGroupDelete(notify_flags_);

	vSemaphoreDelete(mutex_);
}

void dispatch_queue::dispatch(const fp_t& op)
{
	BaseType_t status = xSemaphoreTakeRecursive(mutex_, portMAX_DELAY);
	assert(status == pdTRUE && "Failed to lock mutex!");

	q_.push(op);

	status = xSemaphoreGiveRecursive(mutex_);
	assert(status == pdTRUE && "Failed to unlock mutex!");

	// Notifies threads that new work has been added to the queue
	xEventGroupSetBits(notify_flags_, DISPATCH_WAKE_EVT);
}

void dispatch_queue::dispatch(fp_t&& op)
{
	BaseType_t status = xSemaphoreTakeRecursive(mutex_, portMAX_DELAY);
	assert(status == pdTRUE && "Failed to lock mutex!");

	q_.push(std::move(op));

	status = xSemaphoreGiveRecursive(mutex_);
	assert(status == pdTRUE && "Failed to unlock mutex!");

	// Notifies threads that new work has been added to the queue
	xEventGroupSetBits(notify_flags_, DISPATCH_WAKE_EVT);
}

void dispatch_queue::dispatch_thread_handler(void)
{
	BaseType_t status = xSemaphoreTakeRecursive(mutex_, portMAX_DELAY);
	assert(status == pdTRUE && "Failed to lock mutex!");

	do {
		//after wait, we own the lock
		if(!quit_ && q_.size())
		{
			auto op = std::move(q_.front());
			q_.pop();

			//unlock now that we're done messing with the queue
			status = xSemaphoreGiveRecursive(mutex_);
			assert(status == pdTRUE && "Failed to unlock mutex!");

			op();

			status = xSemaphoreTakeRecursive(mutex_, portMAX_DELAY);
			assert(status == pdTRUE && "Failed to lock mutex!");
		}
		else if(!quit_)
		{
			status = xSemaphoreGiveRecursive(mutex_);
			assert(status == pdTRUE && "Failed to unlock mutex!");

			// Wait for new work - clear flags on exit
			xEventGroupWaitBits(notify_flags_, DISPATCH_WAKE_EVT, pdTRUE, pdFALSE, portMAX_DELAY);

			status = xSemaphoreTakeRecursive(mutex_, portMAX_DELAY);
			assert(status == pdTRUE && "Failed to lock mutex!");
		}
	} while (!quit_);

	// We were holding the mutex after we woke up
	status = xSemaphoreGiveRecursive(mutex_);
	assert(status == pdTRUE && "Failed to unlock mutex!");

	// Set a signal to indicate a thread exited
	status = xEventGroupSetBits(notify_flags_, DISPATCH_EXIT_EVT);
	assert(status == pdTRUE && "Failed to set event flags!");

	// Delete the current thread
	vTaskDelete(NULL);
}
