#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "circular_buffer.h"

/// This implementation is threadsafe for a single producer and single consumer

// The definition of our circular buffer structure is hidden from the user
struct circular_buf_t
{
	uint8_t* buffer;
	size_t head;
	size_t tail;
	size_t max; // of the buffer
};

#pragma mark - Private Functions -

static inline size_t advance_headtail_value(size_t value, size_t max)
{
	if(++value == max)
	{
		value = 0;
	}

	return value;
}

#pragma mark - APIs -

cbuf_handle_t circular_buf_init(uint8_t* buffer, size_t size)
{
	assert(buffer && size > 1);

	cbuf_handle_t cbuf = malloc(sizeof(circular_buf_t));
	assert(cbuf);

	cbuf->buffer = buffer;
	cbuf->max = size;
	circular_buf_reset(cbuf);

	assert(circular_buf_empty(cbuf));

	return cbuf;
}

void circular_buf_free(cbuf_handle_t cbuf)
{
	assert(cbuf);
	free(cbuf);
}

void circular_buf_reset(cbuf_handle_t cbuf)
{
	assert(cbuf);

	cbuf->head = 0;
	cbuf->tail = 0;
}

size_t circular_buf_size(cbuf_handle_t cbuf)
{
	assert(cbuf);

	// We account for the space we can't use for thread safety
	size_t size = cbuf->max - 1;

	if(!circular_buf_full(cbuf))
	{
		if(cbuf->head >= cbuf->tail)
		{
			size = (cbuf->head - cbuf->tail);
		}
		else
		{
			size = (cbuf->max + cbuf->head - cbuf->tail);
		}
	}

	return size;
}

size_t circular_buf_capacity(cbuf_handle_t cbuf)
{
	assert(cbuf);

	// We account for the space we can't use for thread safety
	return cbuf->max - 1;
}

/// For thread safety, do not use put - use try_put.
/// Because this version, which will overwrite the existing contents
/// of the buffer, will involve modifying the tail pointer, which is also
/// modified by get.
void circular_buf_put(cbuf_handle_t cbuf, uint8_t data)
{
	assert(cbuf && cbuf->buffer);

	cbuf->buffer[cbuf->head] = data;
	if(circular_buf_full(cbuf))
	{
		// THIS CONDITION IS NOT THREAD SAFE
		cbuf->tail = advance_headtail_value(cbuf->tail, cbuf->max);
	}

	cbuf->head = advance_headtail_value(cbuf->head, cbuf->max);
}

int circular_buf_try_put(cbuf_handle_t cbuf, uint8_t data)
{
	assert(cbuf && cbuf->buffer);

	int r = -1;

	if(!circular_buf_full(cbuf))
	{
		cbuf->buffer[cbuf->head] = data;
		cbuf->head = advance_headtail_value(cbuf->head, cbuf->max);
		r = 0;
	}

	return r;
}

int circular_buf_get(cbuf_handle_t cbuf, uint8_t* data)
{
	assert(cbuf && data && cbuf->buffer);

	int r = -1;

	if(!circular_buf_empty(cbuf))
	{
		*data = cbuf->buffer[cbuf->tail];
		cbuf->tail = advance_headtail_value(cbuf->tail, cbuf->max);
		r = 0;
	}

	return r;
}

bool circular_buf_empty(cbuf_handle_t cbuf)
{
	assert(cbuf);
	return cbuf->head == cbuf->tail;
}

bool circular_buf_full(circular_buf_t* cbuf)
{
	// We want to check, not advance, so we don't save the output here
	return advance_headtail_value(cbuf->head, cbuf->max) == cbuf->tail;
}

int circular_buf_peek(cbuf_handle_t cbuf, uint8_t* data, unsigned int look_ahead_counter)
{
	int r = -1;
	size_t pos;

	assert(cbuf && data && cbuf->buffer);

	// We can't look beyond the current buffer size
	if(circular_buf_empty(cbuf) || look_ahead_counter > circular_buf_size(cbuf))
	{
		return r;
	}

	pos = cbuf->tail;
	for(unsigned int i = 0; i < look_ahead_counter; i++)
	{
		data[i] = cbuf->buffer[pos];
		pos = advance_headtail_value(pos, cbuf->max);
	}

	return 0;
}
