#ifndef INTERRUPT_MANAGER_HPP_
#define INTERRUPT_MANAGER_HPP_

#include <cstdint>

template<typename TDerived, typename TIRQIDType = int8_t, typename TPriorityType = uint8_t>
class InterruptManagerBase
{
  public:
	static TPriorityType priority(TIRQIDType irq) noexcept
	{
		return TDerived::priority_(irq);
	}

	static TPriorityType priority(TIRQIDType irq, TPriorityType priority) noexcept
	{
		return TDerived::priority_(irq, priority);
	}

	static void enableInterrupts() noexcept
	{
		TDerived::enableInterrupts_();
	}

	static void disableInterrupts() noexcept
	{
		TDerived::disableInterrupts_();
	}

	static void enable(TIRQIDType irq) noexcept
	{
		TDerived::enable_(irq);
	}

	static void disable(TIRQIDType irq) noexcept
	{
		TDerived::disable_(irq);
	}

	static bool enabled(TIRQIDType irq) noexcept
	{
		return TDerived::enabled_(irq);
	}

	static bool pending(TIRQIDType irq) noexcept
	{
		return TDerived::pending_(irq);
	}

	static void clear(TIRQIDType irq) noexcept
	{
		TDerived::clear_(irq);
	}

	static uintptr_t handler(TIRQIDType irq, uintptr_t func) noexcept
	{
		return TDerived::handler_(irq, func);
	}

	static uintptr_t handler(TIRQIDType irq) noexcept
	{
		return TDerived::handler_(irq);
	}

	/// Deleted copy constructor
	InterruptManagerBase(const InterruptManagerBase&) = delete;

	/// Deleted copy assignment operator
	const InterruptManagerBase& operator=(const InterruptManagerBase&) = delete;

	/// Deleted move constructor
	InterruptManagerBase(InterruptManagerBase&&) = delete;

	/// Deleted move assignment operator
	InterruptManagerBase& operator=(InterruptManagerBase&&) = delete;

  protected:
	InterruptManagerBase() = default;
	~InterruptManagerBase() = default;
};

#endif // INTERRUPT_MANAGER_HPP_
