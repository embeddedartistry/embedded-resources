#include <cstdint>
#include <iostream>

#define LOCATION_IMPL(CNT) \
    []{ \
        __asm("__metal_serial_" #CNT ":" ); \
        extern const std::uintptr_t __code_location ## CNT __asm("__metal_serial_" #CNT);   \
        return & __code_location ## CNT; \
    }()

#define LOCATION_IMPL2(CNT) LOCATION_IMPL(CNT)
#define LOCATION() LOCATION_IMPL2(__COUNTER__)

int main()
{
  	std::cout << "main: " << reinterpret_cast<void*>(&main) << std::endl;
	auto loc = LOCATION();
	std::cout << "location: " << loc << std::endl;
	return 0;
}
