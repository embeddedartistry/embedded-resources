#include "nvic.hpp"
#include <processor_includes.hpp>

void NVICControl::trigger(int32_t irq) noexcept
{
	NVIC_SetPendingIRQ(static_cast<IRQn_Type>(irq));
}

uint32_t NVICControl::priority_(int32_t irq) noexcept
{
	return NVIC_GetPriority(static_cast<IRQn_Type>(irq));
}

uint32_t NVICControl::priority_(int32_t irq, uint32_t priority) noexcept
{
	NVIC_SetPriority(static_cast<IRQn_Type>(irq), priority);

	return priority;
}

void NVICControl::enable_(int32_t irq) noexcept
{
	NVIC_EnableIRQ(static_cast<IRQn_Type>(irq));
}

void NVICControl::disable_(int32_t irq) noexcept
{
	NVIC_DisableIRQ(static_cast<IRQn_Type>(irq));
}

bool NVICControl::enabled_(int32_t irq) noexcept
{
	return 0 != (NVIC->ISER[irq / 32] & (1UL << (irq % 32)));
}

bool NVICControl::pending_(int32_t irq) noexcept
{
	return (NVIC_GetPendingIRQ(static_cast<IRQn_Type>(irq)) == 1);
}

void NVICControl::clear_(int32_t irq) noexcept
{
	NVIC_ClearPendingIRQ(static_cast<IRQn_Type>(irq));
}

uintptr_t NVICControl::handler_(int32_t irq, uintptr_t func) noexcept
{
	NVIC_SetVector(static_cast<IRQn_Type>(irq), static_cast<uint32_t>(func));

	return func;
}

uintptr_t NVICControl::handler_(int32_t irq) noexcept
{
	return NVIC_GetVector(static_cast<IRQn_Type>(irq));
}
