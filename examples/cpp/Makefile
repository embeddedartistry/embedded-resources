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

all: callbacks dispatch shared_ptr aligned_ptr circular_buffer

circular_buffer: circular_buffer.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking circular_buffer example
	$(Q) $(CXX) $(LDFLAGS) $(addprefix $(BUILDRESULTS),$^) -o $(BUILDRESULTS)/$@

callbacks: callbacks.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking callbacks example
	$(Q) $(CXX) $(LDFLAGS) $(addprefix $(BUILDRESULTS),$^) -o $(BUILDRESULTS)/$@

dispatch: dispatch.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking dispatch example
	$(Q) $(CXX) $(LDFLAGS) $(BUILDRESULTS)/$< -o $(BUILDRESULTS)/$@

shared_ptr: shared_ptr.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking shared_ptr example
	$(Q) $(CXX) $(LDFLAGS) $(BUILDRESULTS)/$< -o $(BUILDRESULTS)/$@

aligned_ptr: smart_ptr_aligned.o malloc_aligned
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking Aligned Smart Pointer example
	$(Q) $(CXX) $(LDFLAGS) $(BUILDRESULTS)/$< $(BUILDRESULTS)/malloc_aligned.o -o $(BUILDRESULTS)/$@

malloc_aligned:
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Compiling aligned malloc C example
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(BUILDRESULTS)/malloc_aligned.o ../c/malloc_aligned.c

.PHONY: clean
clean:
	$(Q)rm -rf $(BUILDRESULTS)
