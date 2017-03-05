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
include $(BUILDTOP)/build/rules.mk

all: libc

libc: malloc_aligned.o malloc_freelist.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Generating libc.a
	$(Q)$(AR) crs $(BUILDRESULTS)/libc.a $(addprefix $(BUILDRESULTS),$^)

clean:
	$(Q)rm -rf $(BUILDRESULTS)
