// Copyright © 2019 Embedded Artistry LLC.
// License: MIT. See LICENSE file for details.

#ifndef ENDIAN_HPP_
#define ENDIAN_HPP_

#include <algorithm>
#include <array>
#include <climits>

namespace embutil
{
/// @defgroup EndianSwap Endian Swap
/// @brief Functions that can be used to swap the endianness of a value.
/// @ingroup FrameworkUtils
/// @{

#if __cplusplus == 201703L

/** Fold Expression Variant of constexpr byteswap
 *
 * Users should not manually invoke this function. It is used internally by byteswap().
 */
template<class T, std::size_t... N>
constexpr T byteswap_impl(T i, std::index_sequence<N...>) noexcept // NOLINT
{
	return static_cast<T>(((((i >> (N * CHAR_BIT)) & static_cast<T>(static_cast<unsigned char>(-1)))
							<< ((sizeof(T) - 1 - N) * CHAR_BIT)) |
						   ...));
};

/** C++17 constexpr byteswap
 *
 * Performs a constexpr byteswap using fold expressions.
 *
 * @tparam T The type of the value being byteswap'd. This is (typically) deduced by the compiler.
 * @tparam U An unsigned representation corresponding to type T.
 * @param i The value to byteswap.
 * @returns Endian swapped value as an unsigned number.
 */
template<class T, class U = typename std::make_unsigned<T>::type>
constexpr U byteswap(T i) noexcept
{
	return byteswap_impl<U>(i, std::make_index_sequence<sizeof(T)>{});
}

#else
/** Recursive constexpr byteswap implementation without C++17 fold expressions.
 *
 * Specialized implementations can be defined for specific types if desired.
 * Example of template specialization for a specific type:
 *
 * @code
 * template<>
 * void SwapEndian<std::uint32_t>(std::uint32_t &value) {
 *     std::uint32_t tmp = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
 *     value = (tmp << 16) | (tmp >> 16);
 * }
 * @endcode
 *
 * @tparam T The type of the value being byteswap'd. This is (typically) deduced by the compiler.
 * @param i The value to byteswap.
 * @param j Used in the recursive calculation; users should not set this.
 * @param n Used in the recursive calculation; users shoudl not set this.
 * @returns Endian swapped value.
 */
template<class T>
constexpr typename std::enable_if<std::is_unsigned<T>::value, T>::type
	byteswap(T i, T j = 0u, std::size_t n = 0u) noexcept
{
	return n == sizeof(T) ?
			   j :
			   byteswap<T>(i >> CHAR_BIT,
						   static_cast<T>((j << CHAR_BIT) |
										  (i & static_cast<T>(static_cast<unsigned char>(-1)))),
						   n + 1);
}
#endif

/** Non-constexpr Robust Endian Swap
 *
 * 	Described here: https://mklimenko.github.io/english/2018/08/22/robust-endian-swap/
 *	This approach avoids bitmasks/shifts and intrinsics, which can be non-portable and error prone
 *
 * The idea is simple: we define a template function, in which there is a union,
 *	containing both the passed value and the std::array of the corresponding size.
 *	We create two objects of such union and perform the reverse copying from one
 *	to another. All the extra assignments will be optimized out by the compiler,
 *	which will make this code fast and efficient.
 *
 * We use the SFINAE-d version to make sure you won’t pass anything wrong
 *
 * The good thing about this is that we have a template for all the corner cases
 * 	and possible types. But the greatest is that the compiler was able to optimize
 *	both of these functions into one assembly command bswap. Some instructions sets
 *	haven’t got that command and if this will become the bottleneck of your program,
 *	you can make a template specialization for the case you need.
 *
 * @tparam T Type of integer to perform the swap on. This value is (typically) deduced
 *	by the compiler.
 * @param val The value to perform an endian swap on.
 * @param unused An unused parameter, present for use with std::enable_if.
 */
template<typename T>
void SwapEndian(T& val,
				typename std::enable_if<std::is_arithmetic<T>::value, std::nullptr_t>::type unused =
					nullptr) noexcept
{
	(void)unused;
	auto ptr = reinterpret_cast<std::uint8_t*>(&val);
	std::array<std::uint8_t, sizeof(T)> raw_src;
	std::array<std::uint8_t, sizeof(T)> raw_dst;

	for(std::size_t i = 0; i < sizeof(T); ++i)
	{
		raw_src[i] = ptr[i]; // NOLINT
	}

	std::reverse_copy(raw_src.begin(), raw_src.end(), raw_dst.begin());

	for(std::size_t i = 0; i < sizeof(T); ++i)
	{
		ptr[i] = raw_dst[i]; // NOLINT
	}
}

/** Endian swap using undefined behavior.
 *
 * Original version of SwapEndian(). The undefined behavior is due to the use of an inactive
 *	member of the union. This function is efficient, but the undefined behavior makes it risky.
 *
 * @tparam T Type of integer to perform the swap on. This value is (typically) deduced
 *	by the compiler.
 * @param val The value to perform an endian swpa on.
 */
template<typename T>
void SwapEndian_UB(T& val) noexcept
{
	union U
	{
		T val;
		std::array<std::uint8_t, sizeof(T)> raw;
	} src, dst;

	// cppcheck-suppress unreadVariable
	src.val = val;
	std::reverse_copy(src.raw.begin(), src.raw.end(), dst.raw.begin());
	val = dst.val;
}

/// @}

} // namespace embutil

#endif // ENDIAN_HPP_
