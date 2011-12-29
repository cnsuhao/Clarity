###############################################################################
# Required tools:
# make
# gcc, ld, ar, as (as well as cross compilers such as avr-, arm-none-eabi-)
# lcov, genhtml, gcov
# perl
# valgrind
#
# Build flags:
# ARCH=<arch> [default: x86]
# MACH=<mach> [default: default]
# TARGET=<release|debug|coverage> [default: debug]
# LINK=<true|false> [default: false]
#
# Available arch/mach combinations can be found in src/arch/<arch>/mach/<mach>
###############################################################################
.PHONY : main test build clean buildtestlib

LINELENGTH := 80
TABSIZE := 4

export CHECKPATCH := ./src/tools/checkpatch/checkpatch.pl \
	--no-typedef \
	--line=$(LINELENGTH) \
	--tab=$(TABSIZE) \
	--no-tree -q -f

main : test build
	@ cat out/rel/testreport.txt

build :
	@ $(MAKE) -f target.mk

test : buildtestlib
	@ $(MAKE) -f tester.mk

buildtestlib :
	@ $(MAKE) -f target.mk ARCH=x86 MACH=tester TARGET=coverage LINK=false

clean :
	@ rm -rf out
