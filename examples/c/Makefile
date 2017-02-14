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

all: freelist malloc_tx

freelist: malloc_freelist.o malloc_test.o
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q) echo Linking malloc freelist example
	$(Q) $(CC) $(LDFLAGS) $(addprefix $(BUILDRESULTS),$^) -o $(BUILDRESULTS)/$@

malloc_tx: 
	$(Q) [-d "$(BUILDRESULTS)" || mkdir -p $(BUILDRESULTS)
	$(Q) echo Compiling malloc_tx object.  THIS WILL NOT LINK!
	$(CC) $(CPPFLAGS) $(CFLAGS) -fno-builtin -c -o $(BUILDRESULTS)/malloc_threadx.o malloc_threadx.c

clean:
	$(Q)rm -rf $(BUILDRESULTS)
