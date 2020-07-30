# Aardvark adapter Driver Build Definitions

aardvark_driver_files = files(
	'base.cpp',
	'i2c.cpp',
	'vendor/aardvark.c'
	)

aardvark_vendor_include = include_directories('vendor', is_system: true)

aardvark_library_loc = meson.current_source_dir() + '/vendor'

aardvark_driver_lib = static_library('aardvark_driver',
	sources: aardvark_driver_files,
	include_directories: [driver_core_inc, framework_includes, aardvark_vendor_include],
	c_args: '-DAARDVARK_LIBRARY_PATH="' + aardvark_library_loc + '"',
	native: true
)

aardvark_driver_dep = declare_dependency(
	link_with: aardvark_driver_lib,
	include_directories: include_directories('..'),
)
