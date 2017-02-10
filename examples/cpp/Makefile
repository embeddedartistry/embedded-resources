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

all: callbacks dispatch shared_ptr

callbacks: callbacks.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking bad C example
	$(Q) $(CXX) $(LDFLAGS) $(addprefix $(BUILDRESULTS),$^) -o $(BUILDRESULTS)/$@

dispatch: dispatch.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking offsetof example
	$(Q) $(CXX) $(LDFLAGS) $(BUILDRESULTS)/$< -o $(BUILDRESULTS)/$@

shared_ptr: shared_ptr.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking Stack Dir example
	$(Q) $(CXX) $(LDFLAGS) $(BUILDRESULTS)/$< -o $(BUILDRESULTS)/$@

.PHONY: clean
clean:
	$(Q)rm -rf $(BUILDRESULTS)
