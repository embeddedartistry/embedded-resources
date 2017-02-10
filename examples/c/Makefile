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

all: freelist

freelist: malloc_freelist.o malloc_test.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking malloc freelist example
	$(Q) $(CC) $(LDFLAGS) $(addprefix $(BUILDRESULTS),$^) -o $(BUILDRESULTS)/$@

clean:
	$(Q)rm -rf $(BUILDRESULTS)
