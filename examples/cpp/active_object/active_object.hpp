#ifndef ACTIVE_OBJECT_HPP_
#define ACTIVE_OBJECT_HPP_

#include <atomic>
#include <condition_variable>
#include <etl/queue.h>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace embutil
{
/* A base class which adds a processing queue and thread to an object.
 *
 * This class represents an object with its own thread of execution. Clients can enqueue
 * operations or events, and they will be processed asynchronously.
 *
 * Use this class as a base class rather than directly managing a thread in an object.
 *
 * # Implementing an Active Object
 *
 * Classes which should have their own thread of control should derive from the activeObject class.
 * The inheritance declaration must include the storage type definition as a template parameter.
 * For example, the aardvarkI2CMaster driver stores a std::pair containing the necessary
 * I2C transaction data (operation struct and callback function).
 *
 * @code
 * class aardvarkI2CMaster final
 *	: public embvm::i2c::master,
 *	  public embutil::activeObject<std::pair<embvm::i2c::op_t, const embvm::i2c::master::cb_t>>
 * {...};
 * @endcode
 *
 * Derived classes must implement the process_() function. This function is used to handle the input
 * data and perform useful processing.
 *
 * # Using an Active Object
 *
 * Operations are added to the op queue through the enqueue() function. Depending on the class
 * and its intended usage, enqueue() can be called direclty by client code or within the derived
 * class functions.
 *
 * For example, aardvarkI2CMaster calls enqueue() within the I2C transfer request function
 * (transfer_()).
 *
 * @code
 * embvm::i2c::status aardvarkI2CMaster::transfer_(const embvm::i2c::op_t& op, const
 *embvm::i2c::master::cb_t& cb)
 * {
 * 	enqueue({op, cb});
 *
 * 	return embvm::i2c::status::enqueued;
 * }
 * @endcode
 *
 * Enqueued operations are processed on the activeObject thread via the derived classes's process_()
 * function. This is where the useful work happens. In the case of aardvarkI2CMaster,
 * aardvarkI2CMaster::process_() parses the operation, talks to the I2C hardware, and returns
 * the result to the caller via the callback function.
 *
 * # process_() Requirements
 *
 * When an element is popped off the queue, this function is called to handle the element
 * processing. The process_() function is implemented by the derived class.
 *
 * process_() takes one parameter: op. This parameter represents the next operation to process
 * on the active object thread.
 *
 * @tparam TDerivedClass The derived class. This is the CRTP pattern.
 * @tparam TStorageType The type of object which can be enqueued for future processing.
 * @tparam TQueueSize When greater than 0 static memory allocation will be used and the queue
 *	will be fixed at the specified size. 0 indicates dynamic memory will be used.
 * @tparam TLock The lock type to use in the activeObject class.
 * @tparam TCond The condition variable type to use in the activeObject class.
 * @ingroup FrameworkUtils
 */
template<class TDerivedClass, typename TStorageType, size_t TQueueSize = 0,
		 typename TLock = std::mutex, typename TCond = std::condition_variable>
class activeObject
{
	/** Queue type definition.
	 *
	 * The queue is statically allocated when TQueueSize > 0, and dynamically allocated when
	 * TQueueSize == 0.
	 */
	using TQueueType = typename std::conditional<(TQueueSize == 0), std::queue<TStorageType>,
												 etl::queue<TStorageType, TQueueSize>>::type;

  public:
	/** Get the number of operations in the queue.
	 *
	 * @returns the number of queued operations.
	 */
	size_t queuedCount() const noexcept
	{
		return op_queue_.size();
	}

	/** Add an operation to the queue.
	 *
	 * This function can be called directly, or by functions internal to the activeObject.
	 * For example, the "transfer" API enqueues transfer operations.
	 *
	 * This function is marked noexcept because we want the program to terminate if an exception
	 * results from this call. For example, the underlying queue may throw on push.
	 *
	 * @param t The operation data object to enqueue for later processing.
	 */
	bool enqueue(TStorageType t) noexcept
	{
		bool val_postable = false;

		std::unique_lock<TLock> l(lock_);

		if constexpr(TQueueSize == 0)
		{
			val_postable = true;
		}
		else
		{
			val_postable == !op_queue_.full();
			// else, the queue is full and we will not succeed in posting
		}

		if(val_postable)
		{
			op_queue_.push(t);
			cv_.notify_one();
		}

		l.unlock();

		return val_postable;
	}

	/** Shutdown the active object.
	 *
	 * Call this function in the parent class destructor to stop the AO thread operation
	 * before destroying the parent class. This will prevent memory access problems due to race
	 * conidtions during destruction.
	 *
	 * This function is marked noexcept because we want the program to terminate if an exception
	 * results from this call.
	 *
	 * The shutdown process is permanent and cannot be reversed without recreating the object.
	 */
	void shutdown() noexcept
	{
		std::unique_lock<TLock> lock(lock_);
		shutdown_ = true;
		lock.unlock();
		cv_.notify_one();
		thread_.join();
	}

  protected:
	/** Default destructor.
	 *
	 * When the active object is destroyed, the thread is shutdown to prevent any race conditions
	 * during destruction.
	 */
	~activeObject() noexcept
	{
		if(!shutdown_)
		{
			shutdown();
		}
	}

  private:
	/// Queue storage instance.
	TQueueType op_queue_{};
	/// Active object lock instance.
	TLock lock_{};
	/// Active object condition variable instance.
	TCond cv_{};
	/// Flag indicating that the active object should shutdown.
	std::atomic<bool> shutdown_ = false;
	/// Active object thread declaration.
	std::thread thread_ = std::thread(&activeObject::thread_handler, this);

  private:
	/** Active object thread function.
	 *
	 * This function monitors the queue and sleeps until new operations are added
	 * (or shutdown_ is set). Once a new item is added to the queue, the thread wakes, pops the
	 * op off the queue, and calls the derived class's process_() function.
	 *
	 * This function is marked noexcept because we want the program to terminate if an exception
	 * results from this call. The underlying queue may throw on pop, for instance.
	 */
	// cppcheck-suppress unusedPrivateFunction
	void thread_handler() noexcept
	{
		std::unique_lock<TLock> lock(lock_);

		do
		{
			// Wait until we have data or a quit signal
			cv_.wait(lock, [this] {
				return (shutdown_ || !op_queue_.empty());
			});

			// after wait, we own the lock
			if(!shutdown_ && !op_queue_.empty())
			{
				auto op = std::move(op_queue_.front());
				op_queue_.pop();
				lock.unlock();

				static_cast<TDerivedClass*>(this)->process_(op);

				lock.lock();
			}
		} while(!shutdown_);

		lock.unlock();
	}
};

} // namespace embutil

#endif // ACTIVE_OBJECT_HPP_
