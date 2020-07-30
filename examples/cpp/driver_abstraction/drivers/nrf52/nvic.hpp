#ifndef ARM_NVIC_DRIVER_HPP_
#define ARM_NVIC_DRIVER_HPP_

#include <processor/interrupt_manager.hpp>

/** General ARM NVIC Controller Interface
 *
 *
 * This class is not meant to be instantiated directly. Instead, use the static methods.
 */
class NVICControl : public InterruptManagerBase<NVICControl, int32_t, uint32_t>
{
  public:
#pragma mark - New Interfaces -

	static void trigger(int32_t irq) noexcept;

#pragma mark - Interrupt Manager Functions -

	static uint32_t priority_(int32_t irq) noexcept;
	static uint32_t priority_(int32_t irq, uint32_t priority) noexcept;
	static void enableInterrupts_() noexcept;
	static void disableInterrupts_() noexcept;
	static void enable_(int32_t irq) noexcept;
	static void disable_(int32_t irq) noexcept;
	static bool enabled_(int32_t irq) noexcept;
	static bool pending_(int32_t irq) noexcept;
	static void clear_(int32_t irq) noexcept;
	static uintptr_t handler_(int32_t irq, uintptr_t func) noexcept;
	static uintptr_t handler_(int32_t irq) noexcept;

  private:
	NVICControl() = default;
	~NVICControl() = default;
};

#endif // ARM_NVIC_DRIVER_HPP_
