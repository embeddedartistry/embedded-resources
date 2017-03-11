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

STRING_SOURCES := $(wildcard string/*.c)
LIBC_SOURCES := $(wildcard *.c)
STDLIB_SOURCES := $(wildcard stdlib/*.c)
LIBC_OBJ := $(LIBC_SOURCES:.c=.o) $(STRING_SOURCES:.c=.o) $(STDLIB_SOURCES:.c=.o)

all: libc

libc: $(LIBC_OBJ)
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Generating libc.a
	$(Q)$(AR) crs $(BUILDRESULTS)/libc.a $(addprefix $(BUILDRESULTS),$(^F))

clean:
	$(Q)rm -rf $(BUILDRESULTS)
