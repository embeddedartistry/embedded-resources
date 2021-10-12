#include <stdint.h>

// The size of these variables should match the size of the timer value.
uint32_t min_latency = UINT32_MAX;
uint32_t max_latency = 0;
// Note that the type of total_latency is larger than min/max latency.
// This is because total_latency is an accumulator and may overflow.
uint64_t total_latency = 0;

/// Later in your program you can calculate average latency using total_latency / count.
uint32_t count = 0;

__attribute__((weak)) inline uint32_t readTimer()
{
	// Override this function to read from the target timer peripheral.
	// Our advice is to read the raw timer value in as few steps as possible,
	// skipping any conversion steps. This will give you the most accurate look
	// at interrupt latency. The application code (or a spreadsheet) can handle
	// the conversion from count to time for you.
	return 0;
}

// Call this function from the timer interrupt handler
void latency(void)
{
	uint32_t time = readTimer();

	if(time < min_latency)
	{
		min_latency = time;
	}

	if(time > max_latency)
	{
		max_latency = time;
	}

	total_latency += time;
	count++;
}
