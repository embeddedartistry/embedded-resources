#define some variables first

ifeq ($(VERBOSE),1)
export Q :=
export VERBOSE := 1
else
export Q := @
export VERBOSE := 0
endif

BUILDTOP ?= ..
BUILDRESULTS ?= buildresults/

include $(BUILDTOP)/build/compiler.mk
include $(BUILDTOP)/build/rules.mk

all: bad offset_of stack_dir

bad: bad.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking bad C example
	$(Q) $(CC) $(LDFLAGS) $(BUILDRESULTS)/$< -o $(BUILDRESULTS)/$@

offset_of: offset_of.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking offsetof example
	$(Q) $(CC) $(LDFLAGS) $(BUILDRESULTS)/$< -o $(BUILDRESULTS)/$@

stack_dir: stack_dir.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking Stack Dir example
	$(Q) $(CC) $(LDFLAGS) $(BUILDRESULTS)/$< -o $(BUILDRESULTS)/$@

.PHONY: clean
clean:
	$(Q)rm -rf $(BUILDRESULTS)
