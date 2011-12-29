TESTEROUT := out/int/tester
TESTERLIB := out/rel/x86-tester-coverage/libclaritycore.a
TESTERINCLUDE := out/rel/x86-tester-coverage/include/
TESTERCLARITYTEST := $(TESTEROUT)/claritytest
TESTERSOURCE := $(wildcard test/*/*.c)
TESTERDADIR := out/int/arch/x86-tester-coverage/src
TESTERTESTS := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.test, $(TESTERSOURCE)))
TESTERTRACES := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.info, $(TESTERSOURCE)))
TESTERCOVERAGE := out/int/tester/coverage

out/rel/testreport.txt : $(TESTEROUT)/testreport.txt
	@ mkdir -p $(dir $@)
	@ cp $(TESTEROUT)/testreport.txt out/rel/testreport.txt

$(TESTEROUT)/testreport.txt : $(TESTERCOVERAGE).info
	@ lcov -l $<  >> $@
	@ genhtml --num-spaces 4 -q -c test/gcov.css -t "Clarity" -o $(TESTERCOVERAGE) $<

$(TESTERCOVERAGE).info : $(TESTERTRACES)
	@ lcov -q  $(addprefix -a , $(TESTERTRACES)) -o $@

$(TESTEROUT)/%.info : $(TESTEROUT)/%.test
	@ echo "Testing $(notdir $(basename $@))"
	@ lcov -z -d $(TESTERDADIR) 2> /dev/null
	@ -valgrind --error-exitcode=1 --leak-check=yes -q $< ;\
	if [ $$? -eq 0 ] ; then \
		echo "$(notdir $(basename $@)) Passed" >> $(TESTEROUT)/testreport.txt; \
	else \
		echo "$(notdir $(basename $@)) Failed" >> $(TESTEROUT)/testreport.txt; \
	fi ;
	@ lcov -q -c -t $(notdir $(basename $@)) -d $(TESTERDADIR) -o $@	

$(TESTEROUT)/%.test : %.c $(TESTEROUT)/%.c.cp $(TESTERLIB)
	@ mkdir -p $(dir $@)
	$(info Building $(notdir $(basename $@)))
	@ gcc $< $(TESTERLIB) -I$(TESTERINCLUDE) -lgcov -o $@

$(TESTEROUT)/%.c.cp : %.c
	@ mkdir -p $(dir $@)
	$(info Analyzing $(notdir $(basename $@)))
	@ $(CHECKPATCH) $<
	@ touch $@
