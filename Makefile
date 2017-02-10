
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

all: c cpp interview

.PHONY: c
c:
	$(Q)echo Building C Examples
	$(Q)make -C examples/c/

.PHONY: cpp
cpp:
	$(Q)echo Building CPP Examples
	$(Q)make -C examples/cpp/

.PHONY: interview
interview:
	$(Q)echo Building Interview Examples
	$(Q)make -C interview/

.PHONY: clean
clean:
	$(Q)echo Cleaning Build Output
	$(Q)rm -rf $(RESULTSROOT)
