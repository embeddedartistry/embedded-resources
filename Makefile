# you can set this to 1 to see all commands that are being run
VERBOSE ?= 0

ifeq ($(VERBOSE),1)
export Q :=
export VERBOSE := 1
else
export Q := @
export VERBOSE := 0
endif

MESON ?= meson
BUILDRESULTS ?= buildresults
CONFIGURED_BUILD_DEP = $(BUILDRESULTS)/build.ninja

# Override to provide your own settings to the shim
OPTIONS ?=
LTO ?= 0
CROSS ?=
NATIVE ?=
DEBUG ?= 0
SANITIZER ?= none
INTERNAL_OPTIONS =

ifeq ($(LTO),1)
	INTERNAL_OPTIONS += -Db_lto=true -Ddisable-builtins=true
endif

ifneq ($(CROSS),)
	# Split into two strings, first is arch, second is chip
	CROSS_2 := $(subst :, ,$(CROSS))
	INTERNAL_OPTIONS += $(foreach FILE,$(CROSS_2),--cross-file=build/cross/$(FILE).txt)
endif

ifneq ($(NATIVE),)
	# Split into words delimited by :
	NATIVE_2 := $(subst :, ,$(NATIVE))
	INTERNAL_OPTIONS += $(foreach FILE,$(NATIVE_2),--native-file=build/native/$(FILE).txt)
endif

ifeq ($(DEBUG),1)
	INTERNAL_OPTIONS += -Ddebug=true -Doptimization=g
endif

ifneq ($(SANITIZER),none)
	INTERNAL_OPTIONS += -Db_sanitize=$(SANITIZER) -Db_lundef=false
endif

all: default

.PHONY: default
default: | $(CONFIGURED_BUILD_DEP)
	$(Q)ninja -C $(BUILDRESULTS)

.PHONY: test
test: | $(CONFIGURED_BUILD_DEP)
	$(Q)ninja -C $(BUILDRESULTS) test

.PHONY: docs
docs: | $(CONFIGURED_BUILD_DEP)
	$(Q)ninja -C $(BUILDRESULTS) docs

.PHONY: package
package: default docs
	$(Q)ninja -C $(BUILDRESULTS) package
	$(Q)ninja -C $(BUILDRESULTS) package-native

# Manually Reconfigure a target, esp. with new options
.PHONY: reconfig
reconfig:
	$(Q) $(MESON) $(BUILDRESULTS) --reconfigure $(INTERNAL_OPTIONS) $(OPTIONS)

# Runs whenever the build has not been configured successfully
$(CONFIGURED_BUILD_DEP):
	$(Q) $(MESON) setup $(BUILDRESULTS) $(INTERNAL_OPTIONS) $(OPTIONS)

.PHONY: cppcheck
cppcheck: | $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) cppcheck

.PHONY: cppcheck-xml
cppcheck-xml: | $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) cppcheck-xml

.PHONY: complexity
complexity: | $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) complexity

.PHONY: complexity-xml
complexity-xml: | $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) complexity-xml

.PHONY: complexity-full
complexity-full: | $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) complexity-full

.PHONY: scan-build
scan-build: $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) scan-build

.PHONY: tidy
tidy: $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) clang-tidy

.PHONY: sloccount
sloccount: $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) sloccount

.PHONY: sloccount-full
sloccount-full: $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) sloccount-full

.PHONY: sloccount-report
sloccount-report: $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) sloccount-report

.PHONY: sloccount-full-report
sloccount-full-report: $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) sloccount-full-report

.PHONY: vale
vale: $(CONFIGURED_BUILD_DEP)
	$(Q) ninja -C $(BUILDRESULTS) vale

.PHONY: coverage
coverage:
	$(Q)if [ ! -e "$(BUILDRESULTS)/coverage/build.ninja" ]; then meson $(BUILDRESULTS)/coverage $(INTERNAL_OPTIONS) $(OPTIONS) -Db_coverage=true; fi
	$(Q) ninja -C $(BUILDRESULTS)/coverage test
	$(Q) ninja -C $(BUILDRESULTS)/coverage coverage

.PHONY: format
format: $(CONFIGURED_BUILD_DEP)
	$(Q)ninja -C $(BUILDRESULTS) format

.PHONY : format-patch
format-patch: $(CONFIGURED_BUILD_DEP)
	$(Q)ninja -C $(BUILDRESULTS) format-patch

.PHONY: clean
clean:
	$(Q) if [ -d "$(BUILDRESULTS)" ]; then ninja -C buildresults clean; fi

.PHONY: distclean
distclean:
	$(Q) rm -rf $(BUILDRESULTS)

### Help Output ###
.PHONY : help
help :
	@echo "usage: make [OPTIONS] <target>"
	@echo "  Options:"
	@echo "    > MESON Override meson tool - useful for testing meson prereleases and forks."
	@echo "    > VERBOSE Show verbose output for Make rules. Default 0. Enable with 1."
	@echo "    > BUILDRESULTS Directory for build results. Default buildresults."
	@echo "    > OPTIONS Configuration options to pass to a build. Default empty."
	@echo "    > LTO Enable LTO builds. Default 0. Enable with 1."
	@echo "    > DEBUG Enable a debug build. Default 0 (release). Enable with 1."
	@echo "    > CROSS Enable a Cross-compilation build. Default format is arch:chip."
	@echo "         - Example: make CROSS=arm:cortex-m3"
	@echo "         - For supported chips, see build/cross/"
	@echo "         - Additional files can be layered by adding additional"
	@echo "           args separated by ':'"
	@echo "			NOTE: cross files in this project will use Embedded Artistry libc/libcpp"
	@echo "    > NATIVE Supply an alternative native toolchain by name."
	@echo "         - Example: make NATIVE=gcc-9"
	@echo "         - Additional files can be layered by adding additional"
	@echo "           args separated by ':'"
	@echo "         - Example: make NATIVE=gcc-9:gcc-gold"
	@echo "    > SANITIZER Compile with support for a Clang/GCC Sanitizer."
	@echo "         Options are: none (default), address, thread, undefined, memory,"
	@echo "         and address,undefined' as a combined option"
	@echo "Targets:"
	@echo "  default: Builds all default targets ninja knows about"
	@echo "  test: Build and run unit test programs"
	@echo "  docs: Generate documentation"
	@echo "  package: Build the project, generates docs, and create a release package"
	@echo "  clean: cleans build artifacts, keeping build files in place"
	@echo "  distclean: removes the configured build output directory"
	@echo "  reconfig: Reconfigure an existing build output folder with new settings"
	@echo "  Code Formating:"
	@echo "    format: runs clang-format on codebase"
	@echo "    format-patch: generates a patch file with formatting changes"
	@echo "  Static Analysis:"
	@echo "    cppcheck: runs cppcheck"
	@echo "    cppcheck-xml: runs cppcheck and generates an XML report (for build servers)"
	@echo "    scan-build: runs clang static analysis"
	@echo "    complexity: runs complexity analysis with lizard, only prints violations"
	@echo "    complexity-full: runs complexity analysis with lizard, prints full report"
	@echo "    complexity-xml: runs complexity analysis with lizard, generates XML report"
	@echo "        (for build servers)"
	@echo "	   sloccount: Run line of code and effort analysis"
	@echo "	   sloccount-full: Run line of code and effort analysis, with results for every file"
	@echo "	   sloccount-report: Run line of code and effort analysis + save to file for Jenkins"
	@echo "	   sloccount-full-report: Run line of code and effort analysis, with results for every file."
	@echo "        Save output to a file for Jenkins"
	@echo "    coverage: runs code coverage analysis and generates an HTML & XML reports"
	@echo "    tidy: runs clang-tidy linter"
	@echo "    vale: lints project documentation against configured style guide"
