#ifndef __ENDIAN_H_
#define __ENDIAN_H_

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __PDP_ENDIAN 3412

#include <_endian.h> //machine endian header

#define BIG_ENDIAN __BIG_ENDIAN
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define PDP_ENDIAN __PDP_ENDIAN
#define BYTE_ORDER __BYTE_ORDER

#include <stdint.h>

#define __bswap16(x) ((uint16_t)((((uint16_t)(x)&0xff00) >> 8) | (((uint16_t)(x)&0x00ff) << 8)))

#define __bswap32(x)                                                                     \
	((uint32_t)((((uint32_t)(x)&0xff000000) >> 24) | (((uint32_t)(x)&0x00ff0000) >> 8) | \
				(((uint32_t)(x)&0x0000ff00) << 8) | (((uint32_t)(x)&0x000000ff) << 24)))

#define __bswap64(x)                                            \
	((uint64_t)((((uint64_t)(x)&0xff00000000000000ULL) >> 56) | \
				(((uint64_t)(x)&0x00ff000000000000ULL) >> 40) | \
				(((uint64_t)(x)&0x0000ff0000000000ULL) >> 24) | \
				(((uint64_t)(x)&0x000000ff00000000ULL) >> 8) |  \
				(((uint64_t)(x)&0x00000000ff000000ULL) << 8) |  \
				(((uint64_t)(x)&0x0000000000ff0000ULL) << 24) | \
				(((uint64_t)(x)&0x000000000000ff00ULL) << 40) | \
				(((uint64_t)(x)&0x00000000000000ffULL) << 56)))

#if __BYTE_ORDER == __LITTLE_ENDIAN

// Definitions from musl libc
#define htobe16(x) __bswap16(x)
#define be16toh(x) __bswap16(x)
#define betoh16(x) __bswap16(x)
#define htobe32(x) __bswap32(x)
#define be32toh(x) __bswap32(x)
#define betoh32(x) __bswap32(x)
#define htobe64(x) __bswap64(x)
#define be64toh(x) __bswap64(x)
#define betoh64(x) __bswap64(x)
#define htole16(x) (uint16_t)(x)
#define le16toh(x) (uint16_t)(x)
#define letoh16(x) (uint16_t)(x)
#define htole32(x) (uint32_t)(x)
#define le32toh(x) (uint32_t)(x)
#define letoh32(x) (uint32_t)(x)
#define htole64(x) (uint64_t)(x)
#define le64toh(x) (uint64_t)(x)
#define letoh64(x) (uint64_t)(x)

// From Apple Open Source Libc
#define ntohs(x) __bswap16(x)
#define htons(x) __bswap16(x)
#define ntohl(x) __bswap32(x)
#define htonl(x) __bswap32(x)
#define ntohll(x) __bswap64(x)
#define htonll(x) __bswap64(x)
#define NTOHL(x) (x) = ntohl((uint32_t)x)
#define NTOHS(x) (x) = ntohs((uint16_t)x)
#define NTOHLL(x) (x) = ntohll((uint64_t)x)
#define HTONL(x) (x) = htonl((uint32_t)x)
#define HTONS(x) (x) = htons((uint16_t)x)
#define HTONLL(x) (x) = htonll((uint64_t)x)

#else // BIG_ENDIAN

// Definitions from musl libc
#define htobe16(x) (uint16_t)(x)
#define be16toh(x) (uint16_t)(x)
#define betoh16(x) (uint16_t)(x)
#define htobe32(x) (uint32_t)(x)
#define be32toh(x) (uint32_t)(x)
#define betoh32(x) (uint32_t)(x)
#define htobe64(x) (uint64_t)(x)
#define be64toh(x) (uint64_t)(x)
#define betoh64(x) (uint64_t)(x)
#define htole16(x) __bswap16(x)
#define le16toh(x) __bswap16(x)
#define letoh16(x) __bswap16(x)
#define htole32(x) __bswap32(x)
#define le32toh(x) __bswap32(x)
#define letoh32(x) __bswap32(x)
#define htole64(x) __bswap64(x)
#define le64toh(x) __bswap64(x)
#define letoh64(x) __bswap64(x)

// From Apple Open Source libc
#define ntohl(x) ((uint32_t)(x))
#define ntohs(x) ((uint16_t)(x))
#define htonl(x) ((uint32_t)(x))
#define htons(x) ((uint16_t)(x))
#define ntohll(x) ((uint64_t)(x))
#define htonll(x) ((uint64_t)(x))

#define NTOHL(x) (x)
#define NTOHS(x) (x)
#define NTOHLL(x) (x)
#define HTONL(x) (x)
#define HTONS(x) (x)
#define HTONLL(x) (x)

#endif // endian check

#endif //__ENDIAN_H_
