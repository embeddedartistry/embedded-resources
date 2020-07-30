project('Embedded Resources',
	['cpp', 'c'],
	default_options : ['cpp_std=c++17'])

#########################
# Process Build Options #
#########################

add_project_arguments('-Wno-unknown-pragmas', language: ['cpp', 'c'])

if get_option('disable-rtti') == true
	add_project_arguments('-fno-rtti', language : 'cpp')
endif

if get_option('disable-exceptions') == true
	add_project_arguments(['-fno-exceptions', '-fno-unwind-tables'], language : 'cpp')
endif

if get_option('enable-werror') == true
	add_project_arguments('-Werror', language : ['cpp', 'c'])
endif

if get_option('enable-pedantic-error') == true
	add_project_arguments('-pedantic-error', language : ['cpp', 'c'])
endif

#TODO: always-enable-assert

###############
# Subprojects #
###############

# Temporarily disabled until the gsl::narrow issue is resolved (#905)
# microsoft_gsl = subproject('microsoft-gsl')
# icrosoft_gsl_dep = microsoft_gsl.get_variable('microsoft_gsl_dep')

#######################
# Process source tree #
#######################s

subdir('examples')
subdir('interview')
