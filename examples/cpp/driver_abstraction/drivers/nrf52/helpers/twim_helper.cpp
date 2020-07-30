#include "twim_helper.hpp"
#include "twim_helper_.hpp"
#include <nordic_nvic.hpp>

using namespace embvm;
using namespace embutil;

// TODO: report error codes with transfers
// TODO: do we report bytes actually sent/received? return int, < 0 = erorr, > 0 = bytes? inout?

/*
 * Useful Datasheet Notes - TWIM section, as of 20190402
 *
 * If a NACK is clocked in from the slave, the TWI master will generate an ERROR event.
 *
 * The .PTR and .MAXCNT registers are double-buffered. They can be updated and prepared for the next
 * RX/TX transmission immediately after having received the RXSTARTED/TXSTARTED event.
 * The STOPPED event indicates that EasyDMA has finished accessing the buffer in RAM.
 *
 * The TWI master will generate a LASTTX event when it starts to transmit the last byte.
 *
 * The TWI master will generate a LASTRX event when it is ready to receive the last byte.
 * If RXD.MAXCNT > 1 the LASTRX event is generated after sending the ACK of the previously received
 * byte. If RXD.MAXCNT = 1 the LASTRX event is generated after receiving the ACK following the
 * address and READ bit.
 *
 * The TWI master is stopped by triggering the STOP task, this task should be triggered during the
 * transmission of the last byte to secure that the TWI will stop as fast as possible after sending
 * the last byte. It is safe to use the shortcut between LASTTX and STOP to accomplish this.
 *
 * Note that the TWI master does not stop by itself when the whole RAM buffer has been sent, or when
 * an error occurs. The STOP task must be issued, through the use of a local or PPI shortcut, or in
 * software as part of the error handler.
 *
 * The TWI master cannot get stopped while it is suspended, so the STOP task has to be issued after
 * the TWI master has been resumed.
 */

// TODO: declare an IRQ0 callback functor and IRQ1 callback functor
// Invoke them from IRQ handler, then clear the value after invoke
// When we want to detect "busy", we can use these handlers to determine "busy"?
// TODO: also need to get it to work with a dispatcher, or bottom half handler

#pragma mark - Local Variables -

namespace
{
nRFTWIM_cb_t twim0_callback;
nRFTWIM_cb_t twim1_callback;
}; // namespace

#pragma mark - Helpers -

static embvm::i2c::status get_converted_error(NRF_TWIM_Type* twi) noexcept
{
	embvm::i2c::status r = embvm::i2c::status::ok;

	auto error = get_errorsrc_(twi);

	bool addr_nack = error & TWIM_ERRORSRC_ANACK_Msk;
	bool data_nack = error & TWIM_ERRORSRC_DNACK_Msk;

	if(addr_nack)
	{
		r = embvm::i2c::status::addrNACK;
	}
	else if(data_nack)
	{
		r = embvm::i2c::status::dataNACK;
	}

	return r;
}

static void invoke_callback_(NRF_TWIM_Type* twi) noexcept
{
	auto error = get_converted_error(twi);

	if(twi == NRF_TWIM0 && twim0_callback)
	{
		twim0_callback(error);
	}
	else if(twi == NRF_TWIM1 && twim1_callback)
	{
		twim1_callback(error);
	}
}

static void twim_irq_handler_(NRF_TWIM_Type* twi) noexcept
{
	if(check_event_(twi, TWIM_EVENT_ERROR))
	{
		handle_error_event_(twi);
		interrupts_enable_(twi, TWIM_INTENSET_STOPPED_Msk);
		if(!check_event_(twi, TWIM_EVENT_STOPPED))
		{
			// If we aren't stopped, wait until we are.
			return;
		}
	}
	else if(check_event_(twi, TWIM_EVENT_STOPPED))
	{
		clear_event_(twi, TWIM_EVENT_STOPPED);
		clear_event_(twi, TWIM_EVENT_LASTTX);
		clear_event_(twi, TWIM_EVENT_LASTRX);
		disable_shortcuts_(twi);
		interrupts_disable_(twi);
	}
	else if(check_event_(twi, TWIM_EVENT_SUSPENDED))
	{
		// We treat suspended like done to allow the no_stop case
		clear_event_(twi, TWIM_EVENT_SUSPENDED);
		disable_shortcuts_(twi);
		interrupts_disable_(twi);
	}
	else
	{
		assert(0);
	}

	invoke_callback_(twi);
}

extern "C" void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler()
{
	twim_irq_handler_(NRF_TWIM0);
}

extern "C" void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler()
{
	twim_irq_handler_(NRF_TWIM1);
}

// TODO: is it cheaper to store a void* in our base class and reinterpret cast every time we use it
// rather than getting the TWI instance with the inst number?
static inline NRF_TWIM_Type* getTWIInst(NordicTWIM inst) noexcept
{
	if(inst == NordicTWIM0)
	{
		return NRF_TWIM0;
	}
	else if(inst == NordicTWIM1)
	{
		return NRF_TWIM1;
	}
	else
	{
		return nullptr;
	}
}

static inline uint32_t getTWIFrequency(embvm::i2c::baud baudrate) noexcept
{
	uint32_t setting = 0;

	// TODO: TWIM_FREQUENCY_FREQUENCY_K250
	switch(baudrate)
	{
		case embvm::i2c::baud::lowSpeed:
			assert(0 && "lowspeed is not supported on NRF I2C");
			break;
		case embvm::i2c::baud::standard:
			setting = TWIM_FREQUENCY_FREQUENCY_K100;
			break;
		case embvm::i2c::baud::fast:
			setting = TWIM_FREQUENCY_FREQUENCY_K400;
			break;
		default:
			assert(0 && "Unknown baudrate");
	}

	return setting;
}

#pragma mark - TWI Translator Functions -

void nRFTWIMTranslator::setCallback(NordicTWIM inst, nRFTWIM_cb_t& cb) noexcept
{
	if(inst == NordicTWIM0)
	{
		twim0_callback = cb;
	}
	else if(inst == NordicTWIM1)
	{
		twim1_callback = cb;
	}
	else
	{
		assert(0);
	}
}

void nRFTWIMTranslator::setSCLPin(NordicTWIM inst, uint8_t port, uint8_t pin) noexcept
{
	auto ctrl = getTWIInst(inst);
	uint32_t value = static_cast<uint32_t>((port << TWIM_PSEL_SCL_PORT_Pos) | pin);
	volatile_store(&ctrl->PSEL.SCL, value);
}

void nRFTWIMTranslator::setSDAPin(NordicTWIM inst, uint8_t port, uint8_t pin) noexcept
{
	auto ctrl = getTWIInst(inst);
	uint32_t value = static_cast<uint32_t>((port << TWIM_PSEL_SCL_PORT_Pos) | pin);
	volatile_store(&ctrl->PSEL.SDA, value);
}

void nRFTWIMTranslator::setFrequency(NordicTWIM inst, embvm::i2c::baud baudrate) noexcept
{
	auto ctrl = getTWIInst(inst);
	auto setting = getTWIFrequency(baudrate);

	volatile_store(&ctrl->FREQUENCY, setting);
}

void nRFTWIMTranslator::enable(NordicTWIM inst) noexcept
{
	auto ctrl = getTWIInst(inst);

	// Enable TWI
	volatile_store(&ctrl->ENABLE,
				   static_cast<uint32_t>(TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos));
}

void nRFTWIMTranslator::disable(NordicTWIM inst) noexcept
{
	auto ctrl = getTWIInst(inst);

	interrupts_disable_(ctrl);
	disable_shortcuts_(ctrl);

	// Disable TWI
	volatile_store(&ctrl->ENABLE,
				   static_cast<uint32_t>(TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos));
}

void nRFTWIMTranslator::enable_interrupts(NordicTWIM inst) noexcept
{
	auto ctrl = getTWIInst(inst);

	NordicNVIC::enable(ctrl);
}

void nRFTWIMTranslator::disable_interrupts(NordicTWIM inst) noexcept
{
	auto ctrl = getTWIInst(inst);

	NordicNVIC::disable(ctrl);
}

void nRFTWIMTranslator::set_interrupt_priority(NordicTWIM inst, uint8_t priority) noexcept
{
	auto ctrl = getTWIInst(inst);

	NordicNVIC::priority(ctrl, priority);
}

void nRFTWIMTranslator::set_transfer_address(NordicTWIM inst, uint8_t address) noexcept
{
	auto ctrl = getTWIInst(inst);
	volatile_store(&ctrl->ADDRESS, static_cast<uint32_t>(address));
}

/** Blocking Transfer Implementation
 *
 * Implementation note:
 *	If you clear the TWIM_EVENET_TXDSENT event, the data that is already loaded into the TX buffer
 *	will be re-sent. You must load the next TX byte and then clear the data.
 */
embvm::i2c::status nRFTWIMTranslator::tx_transfer(NordicTWIM inst, const uint8_t* data,
												  size_t length, bool no_stop) noexcept
{
	auto ctrl = getTWIInst(inst);

	interrupts_disable_(ctrl);

	set_tx_buffer_(ctrl, data, length);

	uint32_t interrupts = TWIM_INTENSET_ERROR_Msk;
	interrupts |= no_stop ? TWIM_INTENSET_SUSPENDED_Msk : TWIM_INTENSET_STOPPED_Msk;

	interrupts_enable_(ctrl, interrupts);

	kickoff_tx(ctrl, no_stop);

	auto ret = get_converted_error(ctrl);
	if(ret == embvm::i2c::status::ok)
	{
		ret = embvm::i2c::status::enqueued;
	}

	return ret;
}

embvm::i2c::status nRFTWIMTranslator::rx_transfer(NordicTWIM inst, uint8_t* data,
												  size_t length) noexcept
{
	auto ctrl = getTWIInst(inst);

	interrupts_disable_(ctrl);

	set_rx_buffer_(ctrl, data, length);

	interrupts_enable_(ctrl,
					   static_cast<uint32_t>(TWIM_INTENSET_STOPPED_Msk | TWIM_INTENSET_ERROR_Msk));

	kickoff_rx(ctrl);

	auto ret = get_converted_error(ctrl);
	if(ret == embvm::i2c::status::ok)
	{
		ret = embvm::i2c::status::enqueued;
	}

	return ret;
}

/** Blocking Transfer Implementation
 *
 * Implementation note:
 *	If you clear the TWIM_EVENET_TXDSENT event, the data that is already loaded into the TX buffer
 *	will be re-sent. You must load the next TX byte and then clear the data.
 */
embvm::i2c::status nRFTWIMTranslator::txrx_transfer(NordicTWIM inst, const uint8_t* tx_data,
													size_t tx_len, uint8_t* rx_data,
													size_t rx_len) noexcept
{
	auto ctrl = getTWIInst(inst);

	interrupts_disable_(ctrl);

	set_tx_buffer_(ctrl, tx_data, tx_len);
	set_rx_buffer_(ctrl, rx_data, rx_len);

	interrupts_enable_(ctrl,
					   static_cast<uint32_t>(TWIM_INTENSET_STOPPED_Msk | TWIM_INTENSET_ERROR_Msk));

	kickoff_txrx(ctrl);

	auto ret = get_converted_error(ctrl);
	if(ret == embvm::i2c::status::ok)
	{
		ret = embvm::i2c::status::enqueued;
	}

	return ret;
}

embvm::i2c::status nRFTWIMTranslator::tx_transfer_blocking(NordicTWIM inst, const uint8_t* data,
														   size_t length, bool no_stop) noexcept
{
	auto ctrl = getTWIInst(inst);

	set_tx_buffer_(ctrl, data, length);

	auto exit_event = kickoff_tx(ctrl, no_stop);
	block_until_complete(ctrl, exit_event);

	return get_converted_error(ctrl);
}

embvm::i2c::status nRFTWIMTranslator::rx_transfer_blocking(NordicTWIM inst, uint8_t* data,
														   size_t length) noexcept
{
	auto ctrl = getTWIInst(inst);

	set_rx_buffer_(ctrl, data, length);

	twi_event_t exit_event = kickoff_rx(ctrl);
	block_until_complete(ctrl, exit_event);

	return get_converted_error(ctrl);
}

embvm::i2c::status nRFTWIMTranslator::txrx_transfer_blocking(NordicTWIM inst,
															 const uint8_t* tx_data, size_t tx_len,
															 uint8_t* rx_data,
															 size_t rx_len) noexcept
{
	auto ctrl = getTWIInst(inst);

	set_tx_buffer_(ctrl, tx_data, tx_len);
	set_rx_buffer_(ctrl, rx_data, rx_len);

	auto exit_event = kickoff_txrx(ctrl);
	block_until_complete(ctrl, exit_event);

	return get_converted_error(ctrl);
}

void nRFTWIMTranslator::stop_condition(NordicTWIM inst) noexcept
{
	auto ctrl = getTWIInst(inst);
	task_trigger_(ctrl, TWIM_TASK_STOP);
}
