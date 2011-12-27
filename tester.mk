TESTEROUT := out/int/tester
TESTERLIB := $(TESTEROUT)/libclaritycore.a
TESTERINCLUDE := $(TESTEROUT)/include/
TESTERCLARITYTEST := $(TESTEROUT)/claritytest
TESTERSOURCE := $(wildcard test/*/*.c)
TESTERDADIR := out/int/arch/x86/tester/coverage/src
TESTERTESTS := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.test, $(TESTERSOURCE)))
TESTERTRACES := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.info, $(TESTERSOURCE)))
TESTERCOVERAGE := out/int/tester/coverage

$(TESTERCOVERAGE) : $(TESTERCOVERAGE).info
	genhtml --num-spaces 4 -c test/gcov.css -q -s -b $(TESTERCOVERAGE)init.info -t "Clarity" -o $@ $@.info

$(TESTERCOVERAGE).info : $(TESTERTRACES) $(TESTERCOVERAGE)init.info
	lcov -q -a $(TESTERCOVERAGE)init.info $(addprefix -a , $(TESTERTRACES)) -o $@

$(TESTERCOVERAGE)init.info : $(TESTERTESTS)
	lcov -q -c -i -d $(TESTERDADIR) -o $@

$(TESTEROUT)/%.info : $(TESTEROUT)/%.test $(TESTERCOVERAGE)init.info
	valgrind --error-exitcode=1 --leak-check=yes -q --dsymutil=yes $<
	lcov -q -c -t $(notdir $(basename $@)) -d $(TESTERDADIR) -o $@

$(TESTEROUT)/%.test : %.c $(TESTERLIB)
	@ mkdir -p $(dir $@)
	gcc $< $(TESTERLIB) -I$(TESTERINCLUDE) -lgcov -o $@

$(TESTERLIB) :
	@ $(MAKE) -f target.mk ARCH=x86 MACH=tester TARGET=coverage OUT=$(TESTEROUT)
