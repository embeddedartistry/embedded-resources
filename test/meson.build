# Test Build Definition

embedded_resources_test_files = files(
	'main.c',
)

#######################
# Test Compiler Flags #
#######################

test_suite_compiler_flags = native_c_compiler.get_supported_arguments(
	'-Wno-unused-parameter',
)

###############
# Test Target #
###############

embedded_resources_tests = executable('embedded_resources_tests',
	embedded_resources_test_files,
	dependencies: [
		cmocka_test_deps,
		cmocka_native_dep,
	],
	link_args: native_map_file.format(meson.current_build_dir() + '/embedded_resources_tests'),
	c_args: test_suite_compiler_flags,
	native: true
)

#############################
# Register Tests with Meson #
#############################

# CMocka Test Suite #

test('embedded_resources_tests',
	embedded_resources_tests,
	env: [
		'CMOCKA_MESSAGE_OUTPUT=XML',
		cmocka_test_output_dir
	])

run_target('embedded-resources-tests',
	command: [embedded_resources_tests]
)

run_target('embedded-resources-tests-tests-xml',
	command: [
		cmocka_with_env_script,
		embedded_resources_tests,
		cmocka_test_output_dir
	],
)
