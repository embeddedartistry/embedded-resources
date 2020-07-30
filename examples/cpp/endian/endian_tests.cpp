#include "endian.hpp"
#include <test.hpp>

using namespace embutil;

TEST_CASE("testing SwapEndian", "[utility/endian]")
{
	uint8_t a = 0xA5;
	SwapEndian(a);
	CHECK(a == 0xA5);

	uint16_t b = 0x5AA5;
	SwapEndian(b);
	CHECK(b == 0xA55A);

	uint32_t c = 0xFFBBEEAA;
	SwapEndian(c);
	CHECK(c == 0xAAEEBBFF);
}

TEST_CASE("testing byteswap (constant)", "[utility/endian]")
{
	const uint8_t a = 0xA5;
	STATIC_REQUIRE(byteswap(a) == 0xA5);

	const uint16_t b = 0x5AA5;
	STATIC_REQUIRE(byteswap(b) == 0xA55A);

	const uint32_t c = 0xFFBBEEAA;
	STATIC_REQUIRE(byteswap(c) == 0xAAEEBBFF);
}

TEST_CASE("testing SwapEndianUB", "[utility/endian]")
{
	uint8_t a = 0xA5;
	SwapEndian_UB(a);
	CHECK(a == 0xA5);

	uint16_t b = 0x5AA5;
	SwapEndian_UB(b);
	CHECK(b == 0xA55A);

	uint32_t c = 0xFFBBEEAA;
	SwapEndian_UB(c);
	CHECK(c == 0xAAEEBBFF);
}
