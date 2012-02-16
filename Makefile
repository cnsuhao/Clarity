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
# egypt
#
# Build flags:
# ARCH=<arch> [default: x86]
# MACH=<mach> [default: default]
# TARGET=<release|debug|coverage> [default: debug]
# LINK=<true|false> [default: false]
# CCCC=<true|false> [default: false]
# EGYPT=<true|false> [default: false]
#
# Available arch/mach combinations can be found in src/arch/<arch>/mach/<mach>
###############################################################################
.PHONY : main test build clean buildtestlib

LINELENGTH := 80
TABSIZE := 8

export CHECKPATCH := perl ./src/tools/checkpatch/checkpatch.pl \
	--no-typedef \
	--line=$(LINELENGTH) \
	--tab=$(TABSIZE) \
	--no-tree -q -f

main: build

build:
	@ $(MAKE) -f target.mk CCCC=false ARQUA=false

profile: buildprofilelib
	@ $(MAKE) -f profiler.mk

test: buildtestlib
	@ $(MAKE) -f tester.mk
	@ cat out/rel/report/test.txt

buildprofilelib:
	@ $(MAKE) -f target.mk ARCH=x86 MACH=profiler TARGET=debug LINK=false CFLAG='-c -O2 -g'

buildtestlib:
	@ $(MAKE) -f target.mk ARCH=x86 MACH=tester TARGET=coverage LINK=false

clean:
	@ rm -rf out
