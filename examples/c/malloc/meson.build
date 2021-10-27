memory_inc = include_directories('.')

libmalloc_threadx = static_library('malloc_threadx',
	'malloc_threadx.c',
	c_args: '-fno-builtin',
	dependencies: threadx_dep
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
	dependencies: linked_list_dep,
	c_args: '-DCOMPILE_AS_EXAMPLE',
)
