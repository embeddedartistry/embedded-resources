# C examples Meson Build File

static_library('interrupt_latency',
	'interrupt_latency.c'
)

executable('circular_buffer',
	[
		'circular_buffer_test.c',
		'circular_buffer/circular_buffer.c'
	],
	include_directories: include_directories('circular_buffer'),
	c_args: '-DCOMPILE_AS_EXAMPLE'
)

executable('malloc_aligned',
	'malloc_aligned.c',
	c_args: '-DCOMPILE_AS_EXAMPLE',
)

executable('malloc_freelist',
	[
		'malloc_freelist.c',
		'malloc_test.c'
	],
	c_args: '-DCOMPILE_AS_EXAMPLE',
)

libmalloc_threadx = static_library('malloc_threadx',
	'malloc_threadx.c',
	c_args: '-fno-builtin',
	include_directories: include_directories('../rtos/threadx')
)
