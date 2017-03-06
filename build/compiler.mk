export CPPFLAGS :=
export WARNINGS := -Wall -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int \
  -Werror-implicit-function-declaration -Wmain -Wparentheses -Wunused \
  -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs \
  -Wuninitialized -Wunknown-pragmas -Wfloat-equal \
  -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings \
  -Wsign-compare -Waggregate-return -Wstrict-prototypes \
  -Wmissing-prototypes -Wmissing-declarations  \
  -Wformat -Wmissing-format-attribute -Wredundant-decls -Wnested-externs \
  -Winline -Wlong-long -Wunreachable-code -Wunused-parameter -Wno-gcc-compat \
  -Wdeprecated
export DISABLED_WARNINGS := -Wno-deprecated-declarations -Wno-empty-body
export ERRORS := -Werror=return-stack-address -Werror=return-type
export CFLAGS := $(CFLAGS) $(WARNINGS) $(ERRORS) $(DISABLED_WARNINGS)
export CXXFLAGS = $(CFLAGS) -std=c++14
export ASFLAGS :=
export LDFLAGS :=
export LDLIBS :=
export INCLUDES :=

ifeq ($(shell clang -v 2>&1 | grep -c "clang version"), 1)
# using normal clang
export CC := clang
export CXX := $(CC)++
export AR := llvm-ar
export AS := llvm-as
ifeq ($(shell brew info llvm 2>&1 | grep -c "Built from source on"), 1)
#we are using a homebrew clang, need new flags
LDFLAGS += -L/usr/local/opt/llvm/lib -Wl,-rpath,/usr/local/opt/llvm/lib
CPPFLAGS += -I/usr/local/opt/llvm/include -I/usr/local/opt/llvm/include/c++/v1/
endif
else
ifeq ($(shell clang -v 2>&1 | grep -c "Apple LLVM version"), 1)
# using apple clang
export CC := clang
export CXX := $(CC)++
export AR := ar
export AS := as
else
ifeq ($(shell gcc -v 2>&1 | grep -c "gcc"), 1)
# using gcc
export CC := gcc
export CXX := g++
export AR := ar
export AS := as
else
$(error Compiler not supported! Please file a bug!)
endif
endif
endif
