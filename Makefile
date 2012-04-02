###############################################################################
# Required build tools:
# make
# gcc, ld, ar, as (as well as cross compilers such as avr-, arm-none-eabi-)
# perl
#
# Required test tools:
# lcov, genhtml, gcov
# valgrind
#
# Required analysis tools:
# cccc
# arqua
#
# Required documentation tools:
# doxygen
#
#
# Build flags:
# ARCH=<arch> [default: x86]
# MACH=<mach> [default: default]
# TARGET=<release|debug|coverage> [default: debug]
# LINK=<true|false> [default: false]
# CCCC=<true|false> [default: false]
# ARQUA=<true|false> [default: false]
#
# Available arch/mach combinations can be found in src/arch/<arch>/mach/<mach>
###############################################################################
.PHONY : main test build clean buildtestlib buildprofilelib buildall doc

LINELENGTH := 80
TABSIZE := 8
PROJECT := clarity

export CHECKPATCH := perl ./src/tools/checkpatch/checkpatch.pl \
	--no-typedef \
	--line=$(LINELENGTH) \
	--tab=$(TABSIZE) \
	--no-tree -q -f

main: build

build:
	@ $(MAKE) -f build/target.mk PROJECT=$(PROJECT)

profile: buildprofilelib
	@ $(MAKE) -f build/profiler.mk

test: buildtestlib
	@ $(MAKE) -f build/tester.mk
	@ cat out/rel/report/test.txt

buildprofilelib:
	@ $(MAKE) -f build/target.mk ARCH=x86 MACH=profiler TARGET=debug \
		LINK=false CFLAG='-c -O2 -g' PROJECT=$(PROJECT)

buildtestlib:
	@ $(MAKE) -f build/target.mk ARCH=x86 MACH=tester TARGET=coverage \
		LINK=false PROJECT=$(PROJECT)

ARCHS := $(notdir $(shell find ./src/arch -type d -depth 1))

buildall:
	@ for target in release debug coverage ; do \
		for arch in $(ARCHS) ; do \
			if [ $$arch != "include" ] ; then \
			femachs=`find ./src/arch/$$arch/mach -type d -depth 1` ; \
			for mach in $$femachs ; do \
			bsmach=`basename $$mach` ; \
			echo "Building ARCH=$$arch MACH=$$bsmach TARGET=$$target" ; \
			$(MAKE) -f build/target.mk CCCC=false ARQUA=false \
			TARGET=$$target ARCH=$$arch MACH=$$bsmach \
			PROJECT=$(PROJECT) LINK=true ; \
			done ; \
			fi ; \
		done ; \
	done

doc:
	$(info Generating Documentation)
	@ mkdir -p out/doc
	@ doxygen build/Doxyfile
	@ $(MAKE) -s -C out/doc/latex pdf

clean:
	@ rm -rf out
