#ifndef NRF_TWIM_HELPER_PRIVATE_HPP_
#define NRF_TWIM_HELPER_PRIVATE_HPP_

#include <nordic_utils.hpp>
#include <processor_includes.hpp>
#include <volatile/volatile.hpp>

#pragma mark - Definitions -

enum twi_task_t
{
	TWIM_TASK_STARTRX = offsetof(NRF_TWIM_Type, TASKS_STARTRX), ///< Start TWI receive sequence.
	TWIM_TASK_STARTTX = offsetof(NRF_TWIM_Type, TASKS_STARTTX), ///< Start TWI transmit sequence.
	TWIM_TASK_STOP = offsetof(NRF_TWIM_Type, TASKS_STOP), ///< Stop TWI transaction.
	TWIM_TASK_SUSPEND = offsetof(NRF_TWIM_Type, TASKS_SUSPEND), ///< Suspend TWI transaction.
	TWIM_TASK_RESUME = offsetof(NRF_TWIM_Type, TASKS_RESUME) ///< Resume TWI transaction.
};

enum twi_event_t
{
	TWIM_EVENT_STOPPED = offsetof(NRF_TWIM_Type, EVENTS_STOPPED), ///< TWI stopped.
	TWIM_EVENT_ERROR = offsetof(NRF_TWIM_Type, EVENTS_ERROR), ///< TWI error.
	TWIM_EVENT_SUSPENDED =
		offsetof(NRF_TWIM_Type,
				 EVENTS_SUSPENDED), ///< TWI entered the suspended state. //0x148 - they hardcoded??
	TWIM_EVENT_RXSTARTED = offsetof(NRF_TWIM_Type, EVENTS_RXSTARTED),
	TWIM_EVENT_TXSTARTED = offsetof(NRF_TWIM_Type, EVENTS_TXSTARTED),
	TWIM_EVENT_LASTRX = offsetof(NRF_TWIM_Type, EVENTS_LASTRX),
	TWIM_EVENT_LASTTX = offsetof(NRF_TWIM_Type, EVENTS_LASTTX)
};

/// All TWI interrupts.
constexpr uint32_t ALL_INTS_MASK = static_cast<uint32_t>(
	TWIM_INTENSET_STOPPED_Msk | TWIM_INTENSET_ERROR_Msk | TWIM_INTENSET_SUSPENDED_Msk |
	TWIM_INTENSET_RXSTARTED_Msk | TWIM_INTENSET_TXSTARTED_Msk | TWIM_INTENSET_LASTRX_Msk |
	TWIM_INTENSET_LASTTX_Msk);

/// All TWI shortcuts.
constexpr uint32_t ALL_SHORTCUTS_MASK = static_cast<uint32_t>(
	TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTTX_SUSPEND_Msk | TWIM_SHORTS_LASTTX_STOP_Msk |
	TWIM_SHORTS_LASTRX_STARTTX_Msk | TWIM_SHORTS_LASTRX_STOP_Msk);

#pragma mark - Local Helpers -

static inline void interrupts_disable_(NRF_TWIM_Type* twi) noexcept
{
	embutil::volatile_store(&twi->INTENCLR, ALL_INTS_MASK);
}

static inline void interrupts_enable_(NRF_TWIM_Type* twi, uint32_t mask) noexcept
{
	embutil::volatile_store(&twi->INTENSET, mask);
}

static inline void disable_shortcuts_(NRF_TWIM_Type* twi) noexcept
{
	auto shorts = embutil::volatile_load(&twi->SHORTS);
	shorts &= ~(ALL_SHORTCUTS_MASK);
	embutil::volatile_store(&twi->SHORTS, shorts);
}

static inline void set_shortcuts_(NRF_TWIM_Type* twi, uint32_t mask) noexcept
{
	embutil::volatile_store(&twi->SHORTS, mask);
}

static inline void clear_event_(NRF_TWIM_Type* twi, twi_event_t event) noexcept
{
	uint32_t* offset = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(twi) +
												   static_cast<uintptr_t>(event));
	embutil::volatile_store(offset, UINT32_C(0));
#if __CORTEX_M == 0x04
	auto dummy = embutil::volatile_load(offset);
	(void)dummy;
#endif
}

static inline bool check_event_(NRF_TWIM_Type* twi, twi_event_t event) noexcept
{
	uint32_t* offset = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(twi) +
												   static_cast<uintptr_t>(event));
	return static_cast<bool>(embutil::volatile_load(offset));
}

static inline void task_trigger_(NRF_TWIM_Type* twi, twi_task_t task) noexcept
{
	uint32_t* offset = reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(twi) +
												   static_cast<uintptr_t>(task));
	embutil::volatile_store(offset, UINT32_C(1));
}

static inline void set_tx_buffer_(NRF_TWIM_Type* twi, const uint8_t* tx_data,
								  size_t length) noexcept
{
	assert(tx_data && length);
	// Easy DMA requires buffers to be in RAM
	assert(is_in_nordic_ram(tx_data));

	embutil::volatile_store(&(twi->TXD.PTR), reinterpret_cast<uint32_t>(tx_data));
	embutil::volatile_store(&(twi->TXD.MAXCNT), static_cast<uint32_t>(length));
}

static inline size_t get_txd_amount_(NRF_TWIM_Type* twi) noexcept
{
	return embutil::volatile_load(&(twi->TXD.AMOUNT));
}

static inline void set_rx_buffer_(NRF_TWIM_Type* twi, uint8_t* rx_data, size_t length) noexcept
{
	assert(rx_data && length);
	// Easy DMA requires buffers to be in RAM
	assert(is_in_nordic_ram(rx_data));

	embutil::volatile_store(&(twi->RXD.PTR), reinterpret_cast<uint32_t>(rx_data));
	embutil::volatile_store(&(twi->RXD.MAXCNT), static_cast<uint32_t>(length));
}

static inline size_t get_rxd_amount_(NRF_TWIM_Type* twi) noexcept
{
	return embutil::volatile_load(&(twi->RXD.AMOUNT));
}

static inline void tx_list_enable_(NRF_TWIM_Type* twi) noexcept
{
	embutil::volatile_store(&(twi->TXD.LIST), UINT32_C(1));
}

static inline void tx_list_disable_(NRF_TWIM_Type* twi) noexcept
{
	embutil::volatile_store(&(twi->TXD.LIST), UINT32_C(0));
}

static inline void rx_list_enable_(NRF_TWIM_Type* twi) noexcept
{
	embutil::volatile_store(&(twi->RXD.LIST), UINT32_C(1));
}

static inline void rx_list_disable_(NRF_TWIM_Type* twi) noexcept
{
	embutil::volatile_store(&(twi->RXD.LIST), UINT32_C(0));
}

static void handle_error_event_(NRF_TWIM_Type* twi) noexcept
{
	clear_event_(twi, TWIM_EVENT_ERROR);

	if(!check_event_(twi, TWIM_EVENT_STOPPED))
	{
		task_trigger_(twi, TWIM_TASK_RESUME);
		task_trigger_(twi, TWIM_TASK_STOP);
	}
}

static void block_until_complete(NRF_TWIM_Type* twi, twi_event_t exit_event) noexcept
{
	while(!check_event_(twi, exit_event))
	{
		auto error = check_event_(twi, TWIM_EVENT_ERROR);
		if(error)
		{
			handle_error_event_(twi);

			// Error detected - stop
			exit_event = TWIM_EVENT_STOPPED;
		}
	}
}

static inline void clear_events_prior_to_kickoff(NRF_TWIM_Type* twi) noexcept
{
	clear_event_(twi, TWIM_EVENT_STOPPED);
	clear_event_(twi, TWIM_EVENT_ERROR);
}

static twi_event_t kickoff_rx(NRF_TWIM_Type* twi) noexcept
{
	clear_events_prior_to_kickoff(twi);

	set_shortcuts_(twi, TWIM_SHORTS_LASTRX_STOP_Msk);

	task_trigger_(twi, TWIM_TASK_RESUME);
	task_trigger_(twi, TWIM_TASK_STARTRX);

	return TWIM_EVENT_STOPPED;
}

static twi_event_t kickoff_tx(NRF_TWIM_Type* twi, bool no_stop) noexcept
{
	auto exit_event = TWIM_EVENT_STOPPED;

	clear_events_prior_to_kickoff(twi);

	if(no_stop)
	{
		set_shortcuts_(twi, TWIM_SHORTS_LASTTX_SUSPEND_Msk);
		clear_event_(twi, TWIM_EVENT_SUSPENDED); // TODO: do we need?

		exit_event = TWIM_EVENT_SUSPENDED;
	}
	else
	{
		set_shortcuts_(twi, TWIM_SHORTS_LASTTX_STOP_Msk);
	}

	task_trigger_(twi, TWIM_TASK_RESUME);
	task_trigger_(twi, TWIM_TASK_STARTTX);

	return exit_event;
}

static twi_event_t kickoff_txrx(NRF_TWIM_Type* twi) noexcept
{
	clear_events_prior_to_kickoff(twi);

	set_shortcuts_(twi, TWIM_SHORTS_LASTTX_STARTRX_Msk | TWIM_SHORTS_LASTRX_STOP_Msk);

	task_trigger_(twi, TWIM_TASK_RESUME);
	task_trigger_(twi, TWIM_TASK_STARTTX);

	return TWIM_EVENT_STOPPED;
}

static inline uint32_t get_errorsrc_(NRF_TWIM_Type* twi) noexcept
{
	auto error = embutil::volatile_load(&twi->ERRORSRC);

	// We write to clear
	embutil::volatile_store(&twi->ERRORSRC, error);

	return error;
}

#endif NRF_TWIM_HELPER_PRIVATE_HPP_
