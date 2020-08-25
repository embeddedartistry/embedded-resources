#include <thread>
#include <functional>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <queue>
#include <mutex>
#include <string>
#include <condition_variable>

class dispatch_queue {
	typedef std::function<void(void)> fp_t;

public:
	dispatch_queue(std::string name, size_t thread_cnt = 1);
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
	std::mutex lock_;
	std::vector<std::thread> threads_;
	std::queue<fp_t> q_;
	std::condition_variable cv_;
	bool quit_ = false;

	void dispatch_thread_handler(void);
};

dispatch_queue::dispatch_queue(std::string name, size_t thread_cnt) :
	name_{std::move(name)}, threads_(thread_cnt)
{
	printf("Creating dispatch queue: %s\n", name_.c_str());
	printf("Dispatch threads: %zu\n", thread_cnt);

	for(size_t i = 0; i < threads_.size(); i++)
	{
		threads_[i] = std::thread(&dispatch_queue::dispatch_thread_handler, this);
	}
}

dispatch_queue::~dispatch_queue()
{
	printf("Destructor: Destroying dispatch threads...\n");

	// Signal to dispatch threads that it's time to wrap up
	std::unique_lock<std::mutex> lock(lock_);
	quit_ = true;
	lock.unlock();
	cv_.notify_all();

	// Wait for threads to finish before we exit
	for(size_t i = 0; i < threads_.size(); i++)
	{
		if(threads_[i].joinable())
		{
			printf("Destructor: Joining thread %zu until completion\n", i);
			threads_[i].join();
		}
	}
}

void dispatch_queue::dispatch(const fp_t& op)
{
	std::unique_lock<std::mutex> lock(lock_);
	q_.push(op);

	// Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
	lock.unlock();
	cv_.notify_one();
}

void dispatch_queue::dispatch(fp_t&& op)
{
	std::unique_lock<std::mutex> lock(lock_);
	q_.push(std::move(op));

	// Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
	lock.unlock();
	cv_.notify_one();
}

void dispatch_queue::dispatch_thread_handler(void)
{
	std::unique_lock<std::mutex> lock(lock_);

	do {
		//Wait until we have data or a quit signal
		cv_.wait(lock, [this]{
			return (q_.size() || quit_);
		});

		//after wait, we own the lock
		if(!quit_ && q_.size())
		{
			auto op = std::move(q_.front());
			q_.pop();

			//unlock now that we're done messing with the queue
			lock.unlock();

			op();

			lock.lock();
		}
	} while (!quit_);
}

int main(void)
{
	int r = 0;
	dispatch_queue q("Phillip's Demo Dispatch Queue", 4);

	q.dispatch([]{printf("Dispatch 1!\n");});
	q.dispatch([]{printf("Dispatch 2!\n");});
	q.dispatch([]{printf("Dispatch 3!\n");});
	q.dispatch([]{printf("Dispatch 4!\n");});

	return r;
}
