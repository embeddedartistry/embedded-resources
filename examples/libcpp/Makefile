ifeq ($(VERBOSE),1)
export Q :=
export VERBOSE := 1
else
export Q := @
export VERBOSE := 0
endif

BUILDTOP ?= ../..
BUILDRESULTS ?= buildresults

include $(BUILDTOP)/build/compiler.mk

# Compiler.mk defines these rules, need to add to them after
#ARCH := -arch armv7
CPPFLAGS += -D_LIBCPP_HAS_NO_THREADS -D_LIBCPP_NO_EXCEPTIONS
CXXFLAGS += -fno-rtti -fno-exceptions
CXXFLAGS += $(ARCH) -fno-builtin -fno-stack-protector -ffreestanding -static -nodefaultlibs
LDFLAGS += $(ARCH) -L$(shell pwd)/$(BUILDRESULTS) -L$(BUILDTOP)/buildresults/ -nodefaultlibs -nostartfiles
#CFLAGS += -pipe -fPIC? -ffreestanding?

include $(BUILDTOP)/build/rules.mk

all: libcpp example

libcpp: new.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)echo Generating libcpp.a
	$(Q)$(AR) crs $(BUILDRESULTS)/libcpp.a $(addprefix $(BUILDRESULTS)/,$^)

example: main.o libcpp
	$(Q)echo Building libcpp example
	$(Q) $(CXX) -v $(LDFLAGS) -lcpp -lc $(BUILDRESULTS)/main.o -o $(BUILDRESULTS)/$@

.PHONY: clean
clean:
	$(Q)rm -rf $(BUILDRESULTS)
