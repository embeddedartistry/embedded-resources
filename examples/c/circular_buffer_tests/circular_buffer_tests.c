/*
 * Copyright © 2021 Embedded Artistry LLC.
 * See LICENSE file for licensing information.
 */

// Cmocka needs these
// clang-format off
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
// clang-format on

#include "circular_buffer/circular_buffer.h"

#define CIRCULAR_BUFFER_SIZE 10

static uint8_t circular_buffer_storage_[CIRCULAR_BUFFER_SIZE] = {0};
static cbuf_handle_t handle_ = NULL;

static int circular_buffer_setup(void** __unused state)
{
	handle_ = circular_buf_init(circular_buffer_storage_, CIRCULAR_BUFFER_SIZE);

	return 0;
}

static int circular_buffer_teardown(void** __unused state)
{
	circular_buf_free(handle_);

	return 0;
}

void circular_buffer_init_test(void** __unused state)
{
	assert_non_null(handle_);
	assert_true(circular_buf_empty(handle_));
	assert_false(circular_buf_full(handle_));
#ifdef TEST_WITH_REDUCED_CAPACITY
	// We are using a strategy that reserves an empty slot for thread safety
	assert_int_equal(circular_buf_capacity(handle_), CIRCULAR_BUFFER_SIZE - 1);
#else
	assert_int_equal(circular_buf_capacity(handle_), CIRCULAR_BUFFER_SIZE);
#endif
}

void circular_buf_put_get_test(void** __unused state)
{
	const int capacity = circular_buf_capacity(handle_);
	for(int i = 0; i < capacity; i++)
	{
		circular_buf_put(handle_, i);

		assert_int_equal(i + 1, circular_buf_size(handle_));
	}

	// Check overflow condition
	circular_buf_put(handle_, capacity);
	assert_int_equal(capacity, circular_buf_size(handle_));

	// Check get - we are expecting that one byte has been overwritten
	// so we should see that the data is [1..10] instead of [0..9]
	for(int i = 0; i < capacity; i++)
	{
		uint8_t data;
		circular_buf_get(handle_, &data);
		assert_int_equal(data, i + 1);
	}
}

void circular_buf_try_put_get_test(void** __unused state)
{
	int success;
	const int capacity = circular_buf_capacity(handle_);

	for(int i = 0; i < capacity; i++)
	{
		success = circular_buf_try_put(handle_, i);
		assert_int_equal(success, 0);
		assert_int_equal(i + 1, circular_buf_size(handle_));
	}

	// Check overflow condition
	success = circular_buf_try_put(handle_, capacity);
	assert_int_equal(success, -1);

	// Check get - we are expecting that the previous put failed,
	// so we should see that the data is [0..9]
	for(int i = 0; i < capacity; i++)
	{
		uint8_t data;
		circular_buf_get(handle_, &data);
		assert_int_equal(data, i);
	}
}

void circular_buffer_full_test(void** __unused state)
{
	const int capacity = circular_buf_capacity(handle_);

	for(int i = 0; i < capacity; i++)
	{
		assert_false(circular_buf_full(handle_));
		circular_buf_put(handle_, i);
	}

	assert_true(circular_buf_full(handle_));
}

void circular_buffer_empty_test(void** __unused state)
{
	const int capacity = circular_buf_capacity(handle_);

	assert_true(circular_buf_empty(handle_));

	for(int i = 0; i < capacity; i++)
	{
		circular_buf_put(handle_, i);
		assert_false(circular_buf_empty(handle_));
	}
}

void circular_buffer_get_more_than_stored_test(void** __unused state)
{
	uint8_t data;

	// We will put one and read two

	circular_buf_put(handle_, 1);

	assert_int_equal(0, circular_buf_get(handle_, &data));
	assert_int_equal(data, 1);
	data = 0;
	assert_int_equal(-1, circular_buf_get(handle_, &data));
	assert_int_equal(data, 0);
}

#pragma mark - Public Functions -

int circular_buffer_test_suite(void)
{
	const struct CMUnitTest circular_buffer_tests[] = {
		cmocka_unit_test_setup_teardown(circular_buffer_init_test, circular_buffer_setup,
										circular_buffer_teardown),
		cmocka_unit_test_setup_teardown(circular_buf_put_get_test, circular_buffer_setup,
										circular_buffer_teardown),
		cmocka_unit_test_setup_teardown(circular_buf_try_put_get_test, circular_buffer_setup,
										circular_buffer_teardown),
		cmocka_unit_test_setup_teardown(circular_buffer_full_test, circular_buffer_setup,
										circular_buffer_teardown),
		cmocka_unit_test_setup_teardown(circular_buffer_empty_test, circular_buffer_setup,
										circular_buffer_teardown),
		cmocka_unit_test_setup_teardown(circular_buffer_get_more_than_stored_test,
										circular_buffer_setup, circular_buffer_teardown),
	};

	return cmocka_run_group_tests(circular_buffer_tests, NULL, NULL);
}
