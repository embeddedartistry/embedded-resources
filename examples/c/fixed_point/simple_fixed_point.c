/*
 * Copyright © 2021 Embedded Artistry LLC.
 * See LICENSE file for licensing information.
 */

#include "simple_fixed_point.h"
#include <math.h>
#include <stdio.h>

#define FIXED_POINT_FRACTIONAL_BITS 5

double fixed_to_double(fixed_point_t input)
{
    return ((double)input / (double)(1 << FIXED_POINT_FRACTIONAL_BITS));
}

fixed_point_t double_to_fixed_round(double input)
{
    return (fixed_point_t)(round(input * (1 << FIXED_POINT_FRACTIONAL_BITS)));
}

fixed_point_t double_to_fixed_truncate(double input)
{
    return (fixed_point_t)(input * (1 << FIXED_POINT_FRACTIONAL_BITS));
}

double fixed16_to_double(fixed_point_t input, uint8_t fractional_bits)
{
    return ((double)input / (double)(1 << fractional_bits));
}
