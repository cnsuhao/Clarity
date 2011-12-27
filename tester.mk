TESTEROUT := out/int/tester
TESTERCLARITYLIB := $(TESTEROUT)/libclaritycore.a
TESTERCLARITYINCLUDE := $(TESTEROUT)/include/
TESTERCLARITYTEST := $(TESTEROUT)/claritytest
TESTERREPORT := $(TESTEROUT)/report
TESTERSOURCE := $(wildcard test/core/*.c)

.PHONY : clean

$(TESTERREPORT) : $(TESTERCLARITYTEST)
	$(TESTERCLARITYTEST)
	@ touch $(TESTERREPORT)	

$(TESTERCLARITYTEST) : $(TESTERCLARITYLIB)
	gcc $(abspath $(TESTERSOURCE)) $< -I$(TESTERCLARITYINCLUDE) -lgcov -o $@

$(TESTERCLARITYLIB) :
	@ $(MAKE) -f target.mk ARCH=x86 MACH=default TARGET=coverage OUT=$(TESTEROUT)
