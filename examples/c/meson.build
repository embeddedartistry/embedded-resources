# C examples Meson Build File

linked_list_dep = declare_dependency(
	include_directories: 'linked_list'
)

subdir('fixed_point')
subdir('malloc')

#############
# Libraries #
#############

static_library('interrupt_latency',
	'interrupt_latency.c',
	build_by_default: meson.is_subproject() == false,
)

###############
# Executables #
###############

executable('refactoring_global_data',
	'refactoring_global_data.c',
	build_by_default: meson.is_subproject() == false,
)

###################
# Circular Buffer #
###################

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
