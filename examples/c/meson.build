# C examples Meson Build File

subdir('fixed_point')

#############
# Libraries #
#############

static_library('interrupt_latency',
	'interrupt_latency.c'
)

libmalloc_threadx = static_library('malloc_threadx',
	'malloc_threadx.c',
	c_args: '-fno-builtin',
	include_directories: include_directories('../rtos/threadx')
)

###############
# Executables #
###############

executable('refactoring_global_data',
	'refactoring_global_data.c'
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

#####################
# Test Dependencies #
#####################

circular_buffer_test_dep = declare_dependency(
	sources: files(
		'circular_buffer/circular_buffer.c',
		'circular_buffer_tests/circular_buffer_tests.c'
	),
	include_directories: [
		include_directories('.'),
		include_directories('circular_buffer_tests'),
	]
)

cmocka_test_deps += circular_buffer_test_dep

circular_buffer_no_modulo_test_dep = declare_dependency(
	sources: files(
		'circular_buffer/circular_buffer_no_modulo.c',
		'circular_buffer_tests/circular_buffer_tests.c'
	),
	include_directories: [
		include_directories('.'),
		include_directories('circular_buffer_tests'),
	]
)

circular_buffer_no_modulo_threadsafe_dep = declare_dependency(
	sources: files(
		'circular_buffer/circular_buffer_no_modulo_threadsafe.c',
		'circular_buffer_tests/circular_buffer_tests.c'
	),
	include_directories: [
		include_directories('.'),
		include_directories('circular_buffer_tests'),
	],
	compile_args: '-DTEST_WITH_REDUCED_CAPACITY'
)
