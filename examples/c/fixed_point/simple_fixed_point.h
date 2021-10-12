/*
 * Copyright © 2021 Embedded Artistry LLC.
 * See LICENSE file for licensing information.
 */

#ifndef SIMPLE_FIXED_POINT_H_
#define SIMPLE_FIXED_POINT_H_

#include <stdint.h>

/// Fixed-point Format: 11.5 (16-bit)
typedef int16_t fixed_point_t;
// If your numbers can only be positive, you can use unsigned to increase range
// typedef uint16_t fixed_point_t;

/// Converts 11.5 format -> double
double fixed_to_double(fixed_point_t input);

/// Converts double to 11.5 format, using round()
fixed_point_t double_to_fixed_round(double input);

/// Converts double to 11.5 format, truncating instead of using round()
fixed_point_t double_to_fixed_truncate(double input);

/// Convert 16-bit fixed-point to double using a variable number of fractional bits
/// and truncation instead of rounding
double fixed16_to_double(fixed_point_t input, uint8_t fractional_bits);

#endif //SIMPLE_FIXED_POINT_H_
