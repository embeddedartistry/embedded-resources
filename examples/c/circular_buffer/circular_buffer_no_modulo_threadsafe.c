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

void circular_buf_free(cbuf_handle_t me)
{
	assert(me);
	free(me);
}

void circular_buf_reset(cbuf_handle_t me)
{
	assert(me);

	me->head = 0;
	me->tail = 0;
}

size_t circular_buf_size(cbuf_handle_t me)
{
	assert(me);

	// We account for the space we can't use for thread safety
	size_t size = me->max - 1;

	if(!circular_buf_full(me))
	{
		if(me->head >= me->tail)
		{
			size = (me->head - me->tail);
		}
		else
		{
			size = (me->max + me->head - me->tail);
		}
	}

	return size;
}

size_t circular_buf_capacity(cbuf_handle_t me)
{
	assert(me);

	// We account for the space we can't use for thread safety
	return me->max - 1;
}

/// For thread safety, do not use put - use try_put.
/// Because this version, which will overwrite the existing contents
/// of the buffer, will involve modifying the tail pointer, which is also
/// modified by get.
void circular_buf_put(cbuf_handle_t me, uint8_t data)
{
	assert(me && me->buffer);

	me->buffer[me->head] = data;
	if(circular_buf_full(me))
	{
		// THIS CONDITION IS NOT THREAD SAFE
		me->tail = advance_headtail_value(me->tail, me->max);
	}

	me->head = advance_headtail_value(me->head, me->max);
}

int circular_buf_try_put(cbuf_handle_t me, uint8_t data)
{
	assert(me && me->buffer);

	int r = -1;

	if(!circular_buf_full(me))
	{
		me->buffer[me->head] = data;
		me->head = advance_headtail_value(me->head, me->max);
		r = 0;
	}

	return r;
}

int circular_buf_get(cbuf_handle_t me, uint8_t* data)
{
	assert(me && data && me->buffer);

	int r = -1;

	if(!circular_buf_empty(me))
	{
		*data = me->buffer[me->tail];
		me->tail = advance_headtail_value(me->tail, me->max);
		r = 0;
	}

	return r;
}

bool circular_buf_empty(cbuf_handle_t me)
{
	assert(me);
	return me->head == me->tail;
}

bool circular_buf_full(cbuf_handle_t me)
{
	// We want to check, not advance, so we don't save the output here
	return advance_headtail_value(me->head, me->max) == me->tail;
}

int circular_buf_peek(cbuf_handle_t me, uint8_t* data, unsigned int look_ahead_counter)
{
	int r = -1;
	size_t pos;

	assert(me && data && me->buffer);

	// We can't look beyond the current buffer size
	if(circular_buf_empty(me) || look_ahead_counter > circular_buf_size(me))
	{
		return r;
	}

	pos = me->tail;
	for(unsigned int i = 0; i < look_ahead_counter; i++)
	{
		data[i] = me->buffer[pos];
		pos = advance_headtail_value(pos, me->max);
	}

	return 0;
}
