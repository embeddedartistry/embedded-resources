#ifndef NRF_GPIO_HELPER_HPP_
#define NRF_GPIO_HELPER_HPP_

#include <cstdint>

/** Translation class which handles nRF GPIO Configuration.
 *
 * This represents a bridge pattern: the implementation of the GPIO functions is separated from the
 * main interfaces (nRFGPIOOutput, nRFGPIOInput, etc.).
 *
 * The GPIO function implementations are isolated from this header because we do not want to make
 * the nRF headers accessible from the rest of the system.
 *
 * This class cannot be directly instantiated.
 */
class nRFGPIOTranslator
{
  public:
	static void configure_output(uint8_t port, uint8_t pin) noexcept;
	static void configure_input(uint8_t port, uint8_t pin, uint8_t pull_config) noexcept;
	static void configure_default(uint8_t port, uint8_t pin) noexcept;
	static void configure_i2c(uint8_t port, uint8_t pin) noexcept;
	static void set(uint8_t port, uint8_t pin) noexcept;
	static void clear(uint8_t port, uint8_t pin) noexcept;

  private:
	/// This class can't be instantiated
	nRFGPIOTranslator() = default;
	~nRFGPIOTranslator() = default;
};

#endif // NRF_GPIO_HELPER_HPP_
