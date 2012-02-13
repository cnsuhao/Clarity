TESTEROUT := out/int/tester
TESTERLIB := out/rel/x86-tester-coverage/libclaritycore.a
TESTERINCLUDE := out/rel/x86-tester-coverage/include/
TESTERCLARITYTEST := $(TESTEROUT)/claritytest
TESTERSOURCE := $(wildcard test/*/*.c)
TESTS := $(notdir $(basename $(wildcard test/*/*.c)))
TESTERDADIR := out/int/arch/x86-tester-coverage/src
TESTEROBJ := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.o, $(TESTERSOURCE)))
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
	@ genhtml -q -c test/gcov.css \
		-t "Clarity" -o $@ $<

$(TESTEROUT)/coverage.info: $(TESTEROUT)/coverage-all.info
	@ lcov --remove $< "/usr*" -o $@

$(TESTEROUT)/testreport.txt: $(TESTEROUT)/coverage.info
	@ rm -f $@
	@ for res in $(TESTERRESULTS) ; do \
		if [ -e $$res ] ; then \
			cat $$res >> $@ ; \
		fi ; \
	done
	@ lcov -l $<  >> $@

$(TESTEROUT)/coverage-all.info: $(TESTERTRACES)
	@ lcov -q  -a $(TESTEROUT)/init.info $(addprefix -a , $(TESTERTRACES)) -o $@

$(TESTEROUT)/%.info: $(TESTEROUT)/tester
	@ echo "Testing $(notdir $(basename $@))"
	@ lcov -z -d $(TESTERDADIR) 2> /dev/null
	@ -valgrind --error-exitcode=1 --leak-check=yes -q $< $(notdir $(basename $@)); \
	if [ $$? -eq 0 ]; then \
		echo "$(notdir $(basename $@)) Passed" > $(basename $@).res; \
	else \
		echo "$(notdir $(basename $@)) Failed" > $(basename $@).res; \
	fi ;
	@ lcov -q -c -t $(notdir $(basename $@)) -d $(TESTERDADIR) -o $@

$(TESTEROUT)/%.o: %.c $(TESTEROUT)/%.c.cp
	@ mkdir -p $(dir $@)
	$(info Compiling $(notdir $(basename $@)))
	@ gcc -Wall -Werror -c -pedantic -g -std=c99 $< -I$(TESTERINCLUDE) \
		--coverage -DCLARITY_FILE=\"$(notdir $(basename $@))\" -o $@

$(TESTEROUT)/tester: $(TESTEROBJ) $(TESTEROUT)/init.info $(TESTERLIB)
	@ mkdir -p $(dir $@)
	$(info Linking $(notdir $(basename $@)))
	@ gcc -Wall -Werror -pedantic -g -std=c99 $(TESTEROBJ) $(TESTERLIB) \
		--coverage -o $@

$(TESTEROUT)/init.info: $(TESTERLIB)
	@ mkdir -p $(dir $@)
	@ lcov -q -c -i -d $(TESTERDADIR) -o $@ 2> /dev/null

$(TESTEROUT)/%.c.cp: %.c
	@ mkdir -p $(dir $@)
	$(info Analyzing $(notdir $(basename $@)))
	@ $(CHECKPATCH) $<
	@ touch $@
