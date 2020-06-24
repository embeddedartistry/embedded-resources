#ifndef VOLATILE_LOAD_STORE_HPP_
#define VOLATILE_LOAD_STORE_HPP_

#include <cassert>
#include <type_traits>

namespace embutil
{
/** \defgroup Volatile_Load-Store Volatile load/store
 *
 * These functions promote safer loading and storing of volatile values.
 *
 * To quote Odin Holmes:
 * > There is a scary amount of bare metal devs who think `volatile` means atomic,
 * > because sometimes it happens to mean that on primitive cores.
 *
 * @code
 * volatile int i = 2; //probably atomic
 * i++; //not atomic ...
 * @endcode
 *
 * Instead we must use volatile_load() and volatile_store() to ensure the operations
 * are probably atomic:
 *
 * @code
 * auto r = volatile_load(&i);
 * r++;
 * volatile_store(&i, r);
 * @endcode
 *
 * What are some goals for an implementation of volatile_load() and volatile_store()?
 *
 * - Code for ​volatile_load() and ​volatile_store() is emitted in program
 * - Implementations must not result in elided or fused instances of ​volatile_load()
 *	and volatile_store() order.
 * - volatile_store<T>​ access does not constitute a data race with either a volatile_load()
 *	or a ​volatile_store() if both occur in the same thread
 *
 * @note Implementations are not obliged to provide cross-thread ordering for instances of ​
 *	volatile_load() and ​volatile_store() in the absence of other mechanisms. One such mechanism
 *	is ​`atomic_thread_fence​`.
 *
 *
 * @ingroup FrameworkUtils
 * @{
 */

/** Read from a volatile variable
 *
 * @tparam TType the type of the variable. This will be deduced by the compiler.
 * @note T​Type shall satisfy the requirements of ​TrivallyCopyable​.
 * @param target The pointer to the volatile variable to read from.
 * @returns the value of the volatile variable.
 */
template<typename TType>
constexpr inline TType volatile_load(const TType* target) noexcept
{
	assert(target);
	static_assert(std::is_trivially_copyable<TType>::value,
				  "Volatile load can only be used with trivially copiable types");
	return *static_cast<const volatile TType*>(target);
}

/// Read from a volatile memory location
/// @overload TType volatile_load(TType* target)
template<typename TType>
constexpr inline TType volatile_load(volatile const TType* target) noexcept
{
	assert(target);
	static_assert(std::is_trivially_copyable<TType>::value,
				  "Volatile load can only be used with trivially copiable types");
	return *target;
}

/** Write to a volatile variable
 *
 * Causes the value of ​`*target` to be overwritten with ​`value`.
 *
 * @tparam TType the type of the variable. This will be deduced by the compiler.
 * @note T​Type shall satisfy the requirements of ​TrivallyCopyable​.
 * @param target The pointer to the volatile variable to update.
 * @param value The new value for the volatile variable.
 */
template<typename TType>
inline void volatile_store(TType* target, TType value) noexcept
{
	assert(target);
	static_assert(std::is_trivially_copyable<TType>::value,
				  "Volatile store can only be used with trivially copiable types");
	*static_cast<volatile TType*>(target) = value;
}

/// Write to a volatile memory location
/// @overload void volatile_store(TType* target, TType value)
template<typename TType>
inline void volatile_store(volatile TType* target, TType value) noexcept
{
	assert(target);
	static_assert(std::is_trivially_copyable<TType>::value,
				  "Volatile store can only be used with trivially copiable types");
	*target = value;
}

// end volatile functions
/// @}

} // namespace embutil

#endif // VOLATILE_LOAD_STORE_HPP_
