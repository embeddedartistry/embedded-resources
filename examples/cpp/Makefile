# This makefile is used to provide convenient access to the generated  makefile

# Export all args to act as a passthrough
export

ER_TOP ?= ../../build/

ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo NotDetected')
endif

ifeq ($(detected_OS), "Windows")
	premake_BIN := premake5.exe
else
	premake_BIN := premake5
endif

all: build

.PHONY: build_gen
build_gen:
	@echo "Detected OS: ${detected_OS}. Using Premake binary: build/bin/${detected_OS}/${premake_BIN}"
	@$(ER_TOP)/bin/${detected_OS}/${premake_BIN} --file=premake5.lua gmake

.PHONY: build
build: build_gen
	@make -C buildresults/build/

.PHONY: help
help: build_gen
	@make -C buildresults/build/ help

.PHONY: clean
clean:
ifeq ("$(wildcard buildresults/build/)","")
	@echo "No generated build files: skipping clean"
else
	@make -C buildresults/build/ clean
endif

.PHONY: purify
purify: clean
	@rm -rf buildresults/

.PHONY: regen
regen:
	@$(ER_TOP)/bin/premake5 --file=premake5.lua gmake


