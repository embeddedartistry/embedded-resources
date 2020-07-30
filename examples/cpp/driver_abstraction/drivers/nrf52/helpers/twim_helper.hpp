#ifndef NRF_TWIM_HELPER_HPP_
#define NRF_TWIM_HELPER_HPP_

#include <cstdint>
#include <driver/i2c.hpp>
#include <inplace_function/inplace_function.hpp>

using nRFTWIM_cb_t = stdext::inplace_function<void(embvm::i2c::status)>;

enum NordicTWIM
{
	NordicTWIM0 = 0,
	NordicTWIM1
};

class nRFTWIMTranslator
{
  public:
	static void setSCLPin(NordicTWIM inst, uint8_t port, uint8_t pin) noexcept;
	static void setSDAPin(NordicTWIM inst, uint8_t port, uint8_t pin) noexcept;
	static void setFrequency(NordicTWIM inst, embvm::i2c::baud freq) noexcept;
	static void setCallback(NordicTWIM inst, nRFTWIM_cb_t& cb) noexcept;
	static void disable(NordicTWIM inst) noexcept;
	static void enable(NordicTWIM inst) noexcept;
	static void disable_interrupts(NordicTWIM inst) noexcept;
	static void enable_interrupts(NordicTWIM inst) noexcept;
	static void set_interrupt_priority(NordicTWIM inst, uint8_t priority) noexcept;
	static void set_transfer_address(NordicTWIM inst, uint8_t address) noexcept;
	static embvm::i2c::status txrx_transfer_blocking(NordicTWIM inst, const uint8_t* tx_data,
													 size_t tx_len, uint8_t* rx_data,
													 size_t rx_len) noexcept;
	static embvm::i2c::status tx_transfer_blocking(NordicTWIM inst, const uint8_t* data,
												   size_t length, bool no_stop) noexcept;
	static embvm::i2c::status rx_transfer_blocking(NordicTWIM inst, uint8_t* data, size_t length);
	static embvm::i2c::status txrx_transfer(NordicTWIM inst, const uint8_t* tx_data, size_t tx_len,
											uint8_t* rx_data, size_t rx_len) noexcept;
	static embvm::i2c::status tx_transfer(NordicTWIM inst, const uint8_t* data, size_t length,
										  bool no_stop) noexcept;
	static embvm::i2c::status rx_transfer(NordicTWIM inst, uint8_t* data, size_t length) noexcept;
	static void stop_condition(NordicTWIM inst) noexcept;

	static constexpr bool NO_STOP = true;
	static constexpr bool STOP = false;

  private:
	/// This class can't be instantiated
	nRFTWIMTranslator() = default;
	~nRFTWIMTranslator() = default;
};

#endif NRF_TWIM_HELPER_HPP_
