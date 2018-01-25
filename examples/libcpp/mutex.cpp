//===------------------------- mutex.cpp ----------------------------------===//
//
//					 The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define _LIBCPP_BUILDING_MUTEX
#include "mutex"
#include "limits"
#include "system_error"
#include "cassert"

#if THREADX
#include <threadx/tx_api.h>
#include <threadx/tx_thread.h>
#include <threadx/tx_initialize.h>
extern "C" uint32_t gWait_state;
#elif FREERTOS
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#endif

namespace std {

#ifndef _LIBCPP_HAS_NO_THREADS

#if !THREADING
#error std::mutex needs to be built with _LIBCPP_HAS_NO_THREADS or an THREAD API
#endif //!THREADING

const defer_lock_t  defer_lock = {};
const try_to_lock_t try_to_lock = {};
const adopt_lock_t  adopt_lock = {};

mutex::mutex() _NOEXCEPT
{
#if THREADING
#if THREADX
	if(::TX_THREAD_GET_SYSTEM_STATE() && TX_THREAD_GET_SYSTEM_STATE() <TX_INITIALIZE_IN_PROGRESS)
	{
		assert(0 && "Trying to lock a mutex from a non-thread!");
	}
#endif //THREADX
#endif //THREADING

#if THREADX
	uint8_t status = tx_mutex_create(&__m_, "std::mutex", TX_INHERIT);
	assert(status == TX_SUCCESS && "Couldn't allocate mutex!");
#elif FREERTOS
	__m_ = xSemaphoreCreateMutex();
	assert(__m_ != NULL && "Failed to create mutex!");
#endif
}

mutex::~mutex()
{
#if THREADX
	uint8_t status = tx_mutex_delete(&__m_);
	assert(status == TX_SUCCESS && "Couldn't delete mutex!");
#elif FREERTOS
	vSemaphoreDelete(__m_);
#endif
}

void
mutex::lock()
{
#if THREADING
#if THREADX
	if(::TX_THREAD_GET_SYSTEM_STATE() && TX_THREAD_GET_SYSTEM_STATE() <TX_INITIALIZE_IN_PROGRESS)
	{
		assert(0 && "Trying to lock a mutex from a non-thread!");
	}
#endif //THREADX
#endif //THREADING

#if THREADX
	uint8_t status = tx_mutex_get(&__m_, gWait_state);
	assert(status == TX_SUCCESS && "Couldn't lock mutex!");
#elif FREERTOS
	BaseType_t status = status = xSemaphoreTake(__m_, portMAX_DELAY);
	assert(status == pdTRUE && "Failed to lock mutex!");
#endif
}

bool
mutex::try_lock() _NOEXCEPT
{
#if THREADX
	uint8_t status = tx_mutex_get(&__m_, TX_NO_WAIT);
	//intentional no wait for try_lock

	return(status == TX_SUCCESS);
#elif FREERTOS
	BaseType_t status = status = xSemaphoreTake(__m_, 0);
	//intentional no wait for try_lock

	return(status == pdTRUE);
#endif //THREADX
}

void
mutex::unlock() _NOEXCEPT
{
#if THREADX
	uint8_t status = tx_mutex_put(&__m_);
	assert(status == TX_SUCCESS && "Couldn't lock mutex!");
#elif FREERTOS
	BaseType_t status = status = xSemaphoreGive(__m_);
	assert(status == pdTRUE && "Failed to unlock mutex!");
#endif //THREADX
}

// recursive_mutex
recursive_mutex::recursive_mutex()
{
#if THREADING
#if THREADX
	if(::TX_THREAD_GET_SYSTEM_STATE() && TX_THREAD_GET_SYSTEM_STATE() <TX_INITIALIZE_IN_PROGRESS)
	{
		assert(0 && "Trying to create a recursive mutex from a non-thread!");
	}
#endif //THREADX
#endif //THREADING

#if THREADX
	uint8_t status = tx_mutex_create(&__m_, "std::recursive_mutex", 0);
	assert(status == TX_SUCCESS && "Couldn't allocate recursive mutex!");
#elif FREERTOS
	__m_ = xSemaphoreCreateRecursiveMutex();
	assert(__m_ != NULL && "Failed to create mutex!");
#endif

	return;
}

recursive_mutex::~recursive_mutex()
{
#if THREADX
	uint8_t status = tx_mutex_delete(&__m_);
	assert(status == TX_SUCCESS && "Couldn't delete recursive mutex!");
#elif FREERTOS
	vSemaphoreDelete(__m_);
#endif //THREADX
}

void
recursive_mutex::lock()
{
#if THREADING
#if THREADX
	if(::TX_THREAD_GET_SYSTEM_STATE() && TX_THREAD_GET_SYSTEM_STATE() <TX_INITIALIZE_IN_PROGRESS)
	{
		assert(0 && "Trying to lock a recursive mutex from a non-thread!");
	}
#endif //THREADX
#endif //THREADING

#if THREADX
	//tx mutex is already recursive
	uint8_t status = tx_mutex_get(&__m_, gWait_state);
	assert(status == TX_SUCCESS && "Couldn't lock recursive mutex!");
#elif FREERTOS
	BaseType_t status = status = xSemaphoreTakeRecursive(__m_, portMAX_DELAY);
	assert(status == pdTRUE && "Failed to lock mutex!");
#endif //THREADX
}

void
recursive_mutex::unlock() _NOEXCEPT
{
#if THREADX
	uint8_t status = tx_mutex_put(&__m_);
	assert(status == TX_SUCCESS && "Couldn't unlock recursive mutex!");
#elif FREERTOS
	BaseType_t status = status = xSemaphoreGiveRecursive(__m_);
	assert(status == pdTRUE && "Failed to unlock mutex!");
#endif //THREADX
}

bool
recursive_mutex::try_lock() _NOEXCEPT
{
#if THREADX
	uint8_t status = tx_mutex_get(&__m_, TX_NO_WAIT);
	//intentional no wait for try_lock

	return(status == TX_SUCCESS);
#elif FREERTOS
	BaseType_t status = status = xSemaphoreTakeRecursive(__m_, 0);
	//intentional no wait for try_lock

	return(status == pdTRUE);
#endif //THREADX
}

#endif // !_LIBCPP_HAS_NO_THREADS
};
