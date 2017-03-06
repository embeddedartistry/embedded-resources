ifeq ($(VERBOSE),1)
export Q :=
export VERBOSE := 1
else
export Q := @
export VERBOSE := 0
endif

BUILDTOP ?= ../..
BUILDRESULTS ?= buildresults/
include $(BUILDTOP)/build/compiler.mk

# Compiler.mk defines these rules, need to add to them after
ARCH := -arch armv7
CFLAGS += $(ARCH) -fno-builtin -static -nodefaultlibs
LDFLAGS += $(ARCH) -L$(shell pwd)/$(BUILDRESULTS) -L$(BUILDTOP)/buildresults/ -nodefaultlibs -nostartfiles
#CFLAGS += -pipe -fPIC? -ffreestanding?

include $(BUILDTOP)/build/rules.mk

all: libc

libc: malloc_aligned.o malloc_freelist.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Generating libc.a
	$(Q)$(AR) crs $(BUILDRESULTS)/libc.a $(addprefix $(BUILDRESULTS),$^)

clean:
	$(Q)rm -rf $(BUILDRESULTS)
