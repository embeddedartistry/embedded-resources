
# you can set this to 1 to see all commands that are being run
export VERBOSE := 0

ifeq ($(VERBOSE),1)
export Q :=
export VERBOSE := 1
else
export Q := @
export VERBOSE := 0
endif

export BUILDTOP := $(shell pwd)
export BUILDRESULTS := $(shell pwd)/buildresults/

all: examples

.PHONY: groundwork
groundwork:
	$(Q)if [ -d "$(BUILDRESULTS)" ]; then mkdir -p $(BUILDRESULTS); fi
	$(Q)if [ ! -e "$(BUILDRESULTS)/build.ninja" ]; then meson $(BUILDRESULTS); fi


.PHONY: examples
examples: groundwork
	$(Q)cd $(BUILDRESULTS); ninja

.PHONY: clean
clean:
	$(Q)echo Cleaning Build Output
	$(Q)cd $(BUILDRESULTS); ninja clean

.PHONY: purify
purify:
	$(Q)echo Removing all build output
	$(Q)rm -rf $(BUILDRESULTS)
