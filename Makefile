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
TABSIZE := 4

export CHECKPATCH := ./src/tools/checkpatch/checkpatch.pl \
	--no-typedef \
	--line=$(LINELENGTH) \
	--tab=$(TABSIZE) \
	--no-tree -q -f

main: test build

build:
	@ $(MAKE) -f target.mk CCCC=false ARQUA=false

test: buildtestlib
	@ $(MAKE) -f tester.mk
	@ cat out/rel/report/test.txt

buildtestlib:
	@ $(MAKE) -f target.mk ARCH=x86 MACH=default TARGET=coverage LINK=false

clean:
	@ rm -rf out
