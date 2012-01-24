TESTEROUT := out/int/tester
TESTERLIB := out/rel/x86-default-coverage/libclaritycore.a
TESTERINCLUDE := out/rel/x86-default-coverage/include/
TESTERCLARITYTEST := $(TESTEROUT)/claritytest
TESTERSOURCE := $(wildcard test/*/*.c)
TESTERDADIR := out/int/arch/x86-default-coverage/src
TESTERTESTS := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.test, $(TESTERSOURCE)))
TESTERTRACES := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.info, $(TESTERSOURCE)))
TESTERRESULTS := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.res, $(TESTERSOURCE)))
TESTERCOVERAGE := out/rel/report/coverage

.PHONY : test
.PRECIOUS : $(TESTEROUT)/%.test

test: out/rel/report/test.txt
test: $(TESTERCOVERAGE)

out/rel/report/test.txt: $(TESTEROUT)/testreport.txt
	@ mkdir -p $(dir $@)
	@ cp $(TESTEROUT)/testreport.txt out/rel/report/test.txt

$(TESTERCOVERAGE): $(TESTEROUT)/coverage.info
	@ genhtml --num-spaces 4 -q -c test/gcov.css \
		-t "Clarity" -o $@ $<

$(TESTEROUT)/testreport.txt: $(TESTEROUT)/coverage.info
	@ rm -f $@
	@ for res in $(TESTERRESULTS) ; do \
		if [ -e $$res ] ; then \
			cat $$res >> $@ ; \
		fi ; \
	done
	@ lcov -l $<  >> $@

$(TESTEROUT)/coverage.info: $(TESTERTRACES)
	@ lcov -q  -a $(TESTEROUT)/init.info $(addprefix -a , $(TESTERTRACES)) -o $@

$(TESTEROUT)/%.info: $(TESTEROUT)/%.test
	@ echo "Testing $(notdir $(basename $@))"
	@ lcov -z -d $(TESTERDADIR) 2> /dev/null
	@ -valgrind --error-exitcode=1 --leak-check=yes -q $< ;\
	if [ $$? -eq 0 ] ; then \
		echo "$(notdir $(basename $@)) Passed" > $(basename $@).res; \
	else \
		echo "$(notdir $(basename $@)) Failed" > $(basename $@).res; \
	fi ;
	@ lcov -q -c -t $(notdir $(basename $@)) -d $(TESTERDADIR) -o $@

$(TESTEROUT)/%.test: %.c $(TESTEROUT)/%.c.cp $(TESTEROUT)/init.info $(TESTERLIB)
	@ mkdir -p $(dir $@)
	$(info Building $(notdir $(basename $@)))
	@ gcc -Wall -Werror -pedantic -g -std=c99 $< $(TESTERLIB) -I$(TESTERINCLUDE) \
		-lgcov -DCLARITY_FILE=\"$(notdir $(basename $@))\" -o $@

$(TESTEROUT)/init.info: $(TESTERLIB)
	@ mkdir -p $(dir $@)
	@ lcov -q -c -i -d $(TESTERDADIR) -o $@ 2> /dev/null

$(TESTEROUT)/%.c.cp: %.c
	@ mkdir -p $(dir $@)
	$(info Analyzing $(notdir $(basename $@)))
	@ $(CHECKPATCH) $<
	@ touch $@
