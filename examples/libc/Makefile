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
#ARCH := -arch armv7
CFLAGS += $(ARCH) -fno-builtin -static -nodefaultlibs
LDFLAGS += $(ARCH) -L$(shell pwd)/$(BUILDRESULTS) -L$(BUILDTOP)/buildresults/
LDFLAGS += -static -nodefaultlibs -nostartfiles
LDFLAGS += -Wl,-preload -Wl,-all_load -Wl,-dead_strip -Wl,-prebind
#CFLAGS += -pipe -fPIC? -ffreestanding?

include $(BUILDTOP)/build/rules.mk

STRING_SOURCES := string/memmove.o string/memcmp.o string/memset.o string/memcpy.o
LIBC_SOURCES := malloc_aligned.o malloc_freelist.o memory.o $(STRING_SOURCES)

all: libc

libc: $(LIBC_SOURCES)
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Generating libc.a
	$(Q)$(AR) crs $(BUILDRESULTS)/libc.a $(addprefix $(BUILDRESULTS),$(^F))

clean:
	$(Q)rm -rf $(BUILDRESULTS)
