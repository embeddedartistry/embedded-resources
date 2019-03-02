# libcpp Meson Build File

libcpp_threadx = static_library('cpp_threadx',
    'mutex.cpp',
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
    	'-Wl,-preload',
    	'-Wl,-all_load,',
    	'-Wl,-prebind',
    	],
    include_directories: rtos_includes,
    build_by_default: true,
)

libcpp_freertos = static_library('cpp_freertos',
    'mutex.cpp',
    cpp_args: [
    	'-DFREERTOS=1',
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
    	'-Wl,-preload',
    	'-Wl,-all_load,',
    	'-Wl,-prebind',
    	],
    include_directories: rtos_includes,
    build_by_default: true,
)

libcpp_freertos_constexpr = static_library('cpp_freertos_constexpr',
    'mutex.cpp',
    cpp_args: [
    	'-DFREERTOS=1',
    	'-D_LIBCPP_NO_EXCEPTIONS',
    	'-DTHREADING=1',
    	'-D_LIBCPP_HAS_THREAD_API_EXTERNAL',
    	'-DUSE_CONSTEXPR_MUTEX',
    	'-fno-builtin',
    	'-static',
    	'-nodefaultlibs'
    	],
    link_args: [
    	'-static',
    	'-nodefaultlibs',
    	'-Wl,-preload',
    	'-Wl,-all_load,',
    	'-Wl,-prebind',
    	],
    include_directories: rtos_includes,
    build_by_default: true,
)
