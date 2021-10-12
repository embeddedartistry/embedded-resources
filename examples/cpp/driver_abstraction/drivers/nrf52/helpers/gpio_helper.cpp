#include "gpio_helper.hpp"
#include <processor_includes.hpp>
#include <volatile/volatile.hpp>

/// Implementation of the GPIO drivers is handled here so we can keep the ARM/Nordic headers
/// decoupled from the rest of the system.

#pragma mark - Types and Definitions -

#ifndef NRF_P0
	#define NRF_P0 NRF_GPIO
#endif

static inline NRF_GPIO_Type* portDecode(uint8_t port) noexcept
{
	if(port == 0)
	{
		return NRF_P0;
	}
	else
	{
		return NRF_P1;
	}
}

// #lizard forgives the parameter count, since we can't easily reduce these options.
static inline void configure(uint8_t port, uint8_t pin, uint8_t dir, uint8_t input, uint8_t pull,
							 uint8_t drive, uint8_t sense) noexcept
{
	auto reg = portDecode(port);

	uint32_t value = (static_cast<uint32_t>(dir << GPIO_PIN_CNF_DIR_Pos)) |
					 (static_cast<uint32_t>(input << GPIO_PIN_CNF_INPUT_Pos)) |
					 (static_cast<uint32_t>(pull << GPIO_PIN_CNF_PULL_Pos)) |
					 (static_cast<uint32_t>(drive << GPIO_PIN_CNF_DRIVE_Pos)) |
					 (static_cast<uint32_t>(sense << GPIO_PIN_CNF_SENSE_Pos));

	embutil::volatile_store(&reg->PIN_CNF[pin], value);
}

void nRFGPIOTranslator::configure_output(uint8_t port, uint8_t pin) noexcept
{
	configure(port, pin, GPIO_PIN_CNF_DIR_Output, GPIO_PIN_CNF_INPUT_Disconnect,
			  GPIO_PIN_CNF_PULL_Disabled, GPIO_PIN_CNF_DRIVE_S0S1, GPIO_PIN_CNF_SENSE_Disabled);
}

void nRFGPIOTranslator::configure_input(uint8_t port, uint8_t pin, uint8_t pull_config) noexcept
{
	configure(port, pin, GPIO_PIN_CNF_DIR_Input, GPIO_PIN_CNF_INPUT_Connect, pull_config,
			  GPIO_PIN_CNF_DRIVE_S0S1, GPIO_PIN_CNF_SENSE_Disabled);
}

void nRFGPIOTranslator::configure_default(uint8_t port, uint8_t pin) noexcept
{
	configure(port, pin, GPIO_PIN_CNF_DIR_Input, GPIO_PIN_CNF_INPUT_Disconnect,
			  GPIO_PIN_CNF_PULL_Disabled, GPIO_PIN_CNF_DRIVE_S0S1, GPIO_PIN_CNF_SENSE_Disabled);
}

void nRFGPIOTranslator::configure_i2c(uint8_t port, uint8_t pin) noexcept
{
	configure(port, pin, GPIO_PIN_CNF_DIR_Input, GPIO_PIN_CNF_INPUT_Connect,
			  GPIO_PIN_CNF_PULL_Pullup, GPIO_PIN_CNF_DRIVE_S0D1, GPIO_PIN_CNF_SENSE_Disabled);
}

void nRFGPIOTranslator::set(uint8_t port, uint8_t pin) noexcept
{
	auto reg = portDecode(port);
	embutil::volatile_store(&reg->OUTSET, (UINT32_C(1) << pin));
}

void nRFGPIOTranslator::clear(uint8_t port, uint8_t pin) noexcept
{
	auto reg = portDecode(port);
	embutil::volatile_store(&reg->OUTCLR, (UINT32_C(1) << pin));
}
