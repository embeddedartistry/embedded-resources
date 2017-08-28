# This makefile is used to provide convenient access to the generated  makefile

# Export all args to act as a passthrough
export

ER_TOP ?= ../build/

all: build

.PHONY: build_gen
build_gen:
	@$(ER_TOP)/bin/premake5 --file=premake5.lua gmake

.PHONY: build
build: build_gen
	@make -C buildresults/build/

.PHONY: help
help: build_gen
	@make -C buildresults/build/ help

.PHONY: clean
clean:
	@make -C buildresults/build/ clean

.PHONY: purify
purify: clean
	@rm -rf buildresults/

.PHONY: regen
regen:
	@$(ER_TOP)/bin/premake5 --file=premake5.lua gmake


