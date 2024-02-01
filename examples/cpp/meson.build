# C++ examples Meson Build File

subdir('driver_abstraction')

catch2_tests_dep += declare_dependency(
	sources: files(
		'circular_buffer.cpp'
	)
)

no_braces = meson.get_compiler('cpp').get_supported_arguments('-Wno-missing-braces')

# Doesn't work with GCC 7
if (native_cpp_compiler_id == 'clang' or
	(native_cpp_compiler_id == 'gcc' and native_cpp_compiler.version() > '8'))
	executable('variant_message',
		'variant_message.cpp',
		cpp_args: no_braces
	)
endif

executable('assembly_source_location',
	'assembly_source_location.cpp'
)

executable('dispatch',
	'dispatch.cpp',
)

executable('shared_ptr',
	'shared_ptr.cpp',
)

executable('aligned_ptr',
	[
		'smart_ptr_aligned.cpp',
		'../libc/malloc_aligned.c'
	],
	include_directories: memory_inc
)

executable('array_demo',
	'array.cpp',
)

executable('vector_demo',
	'vector.cpp',
)

executable('string_demo',
	'string.cpp',
)

executable('callbacks',
	'callbacks.cpp',
)

executable('bounce',
	'bounce.cpp',
)

executable('c_sbrm',
	'c_sbrm.cpp',
)

libdispatch_threadx = static_library('dispatch_threadx',
	'dispatch_threadx.cpp',
	include_directories: include_directories('../rtos/threadx'),
)

# This example no longer compiles with the latest clang
# if native_cpp_compiler_id == 'clang'
if false
	libdispatch_threadx_stdmutex = static_library('dispatch_threadx_stdmutex',
		[
			'dispatch_threadx_stdmutex.cpp',
			'../libcpp/mutex.cpp'
		],
		include_directories: [
			include_directories('../rtos'),
			include_directories('../libcpp'),
		],
		cpp_args: [
			'-DTHREADX=1',
			'-D_LIBCPP_NO_EXCEPTIONS',
			'-DTHREADING=1',
			'-D_LIBCPP_HAS_THREAD_API_EXTERNAL',
			'-fno-builtin',
			'-static',
			'-nodefaultlibs'
		],
		link_args: [
			'-static',
			'-nodefaultlibs',
			'-nostartfiles',
			'-Wl,-preload',
			'-Wl,-all_load',
			'-Wl,-dead_strip',
			'-Wl,-prebind'
		]
	)
endif

libdispatch_freertos = static_library('dispatch_freertos',
	'dispatch_freertos.cpp',
	include_directories: include_directories('../rtos')
)
