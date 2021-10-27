/*
 * Copyright © 2021 Embedded Artistry LLC.
 * See LICENSE file for licensing information.
 */

// CMocka needs these
// clang-format off
#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <cmocka.h>
// clang-format on

#include <circular_buffer_tests.h>

int main(void)
{
	int overall_result = 0;

	overall_result |= circular_buffer_test_suite();

	return overall_result;
}
