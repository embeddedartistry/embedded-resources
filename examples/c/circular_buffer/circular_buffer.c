#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "circular_buffer.h"

// The definition of our circular buffer structure is hidden from the user
struct circular_buf_t
{
	uint8_t* buffer;
	size_t head;
	size_t tail;
	size_t max; // of the buffer
	bool full;
};

#pragma mark - Private Functions -

static inline size_t advance_headtail_value(size_t value, size_t max)
{
	return (value + 1) % max;
}

static void advance_head_pointer(cbuf_handle_t cbuf)
{
	assert(cbuf);

	if(circular_buf_full(cbuf))
	{
		cbuf->tail = advance_headtail_value(cbuf->tail, cbuf->max);
	}

	cbuf->head = advance_headtail_value(cbuf->head, cbuf->max);
	cbuf->full = (cbuf->head == cbuf->tail);
}

#pragma mark - APIs -

cbuf_handle_t circular_buf_init(uint8_t* buffer, size_t size)
{
	assert(buffer && size);

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
	cbuf->full = false;
}

size_t circular_buf_size(cbuf_handle_t cbuf)
{
	assert(cbuf);

	size_t size = cbuf->max;

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

	return cbuf->max;
}

void circular_buf_put(cbuf_handle_t cbuf, uint8_t data)
{
	assert(cbuf && cbuf->buffer);

	cbuf->buffer[cbuf->head] = data;

	advance_head_pointer(cbuf);
}

int circular_buf_try_put(cbuf_handle_t cbuf, uint8_t data)
{
	int r = -1;

	assert(cbuf && cbuf->buffer);

	if(!circular_buf_full(cbuf))
	{
		cbuf->buffer[cbuf->head] = data;
		advance_head_pointer(cbuf);
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
		cbuf->full = false;
		r = 0;
	}

	return r;
}

bool circular_buf_empty(cbuf_handle_t cbuf)
{
	assert(cbuf);

	return (!circular_buf_full(cbuf) && (cbuf->head == cbuf->tail));
}

bool circular_buf_full(cbuf_handle_t cbuf)
{
	assert(cbuf);

	return cbuf->full;
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
