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
	@ mkdir -p out/rel
	genhtml --num-spaces 4 -q -c test/gcov.css -b $(TESTERCOVERAGE)init.info -t "Clarity" -o $@ $@.info
	@ cat $(TESTEROUT)/testreport.txt > out/rel/testreport.txt

$(TESTERCOVERAGE).info : $(TESTERTRACES) $(TESTERCOVERAGE)init.info
	@ echo "\nCoverage:" >> $(TESTEROUT)/testreport.txt
	lcov -q -a $(TESTERCOVERAGE)init.info $(addprefix -a , $(TESTERTRACES)) -o $@
	lcov -l $@  >> $(TESTEROUT)/testreport.txt

$(TESTERCOVERAGE)init.info : $(TESTERTESTS)
	@ echo Test Cases: > $(TESTEROUT)/testreport.txt
	lcov -q -c -i -d $(TESTERDADIR) -o $@

$(TESTEROUT)/%.info : $(TESTEROUT)/%.test $(TESTERCOVERAGE)init.info
	-valgrind --error-exitcode=1 --leak-check=yes -q --dsymutil=yes $< ;\
	if [ $$? -eq 0 ] ; then \
		echo $(notdir $(basename $@)) Passed >> $(TESTEROUT)/testreport.txt; \
	else \
		echo $(notdir $(basename $@)) Failed >> $(TESTEROUT)/testreport.txt; \
	fi ;
	lcov -q -c -t $(notdir $(basename $@)) -d $(TESTERDADIR) -o $@
	

$(TESTEROUT)/%.test : %.c $(TESTERLIB)
	@ mkdir -p $(dir $@)
	gcc $< $(TESTERLIB) -I$(TESTERINCLUDE) -lgcov -o $@

$(TESTERLIB) :
	@ $(MAKE) -f target.mk ARCH=x86 MACH=tester TARGET=coverage OUT=$(TESTEROUT)
