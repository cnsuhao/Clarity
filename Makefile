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
.PHONY : main test build clean

main : test build

build :
	@ $(MAKE) -f target.mk

test :
	@ $(MAKE) -f tester.mk

clean :
	rm -rf out
