# This makefile is used to provide convenient access to the generated  makefile

# Export all args to act as a passthrough
export

ER_TOP ?= ../../

all: build

.PHONY: build_gen
build_gen:
	@$(ER_TOP)/bin/premake5 --file=build/premake5.lua gmake

.PHONY: build
build: build_gen
	@make -C build/gen/

.PHONY: help
help: build_gen
	@make -C build/gen/ help

.PHONY: clean
clean:
	@make -C build/gen/ clean
	@rm -rf buildresults

.PHONY: purify
purify: clean
	@rm -rf build/gen

.PHONY: regen
regen:
	@$(ER_TOP)/bin/premake5 --file=build/premake5.lua gmake


