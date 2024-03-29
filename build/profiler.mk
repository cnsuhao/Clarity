TESTEROUT := out/int/profiler
TESTERLIB := out/rel/x86-profiler-debug/libclarity.a
TESTERINCLUDE := out/rel/x86-profiler-debug/include/
TESTERSOURCE := $(wildcard test/core/*.c)
TESTS := $(notdir $(basename $(wildcard test/core/*.c)))
TESTERDADIR := out/int/arch/x86-profiler-debug/src
TESTEROBJ := \
	$(addprefix $(TESTEROUT)/, \
	$(patsubst %.c,%.o, $(TESTERSOURCE)))
TESTERCOVERAGE := out/rel/report/coverage

-include src/arch/x86/mach/profiler/mach.mk
-include src/arch/x86/arch.mk

.PHONY : profile

profile: $(TESTEROUT)/profile.log

$(TESTEROUT)/profile.log: $(TESTEROUT)/callgrind
	@ callgrind_annotate $< > $@
	@ cat $@

$(TESTEROUT)/callgrind: $(TESTEROUT)/profiler
	$(info Profiling)
	@ valgrind --dsymutil=yes --tool=callgrind --callgrind-out-file=$@ -q $< $(TESTS)

$(TESTEROUT)/%.o: %.c
	@ mkdir -p $(dir $@)
	$(info Compiling $(notdir $(basename $@)))
	@ $(CC) -c -g $< -I$(TESTERINCLUDE) \
		-DCLARITY_FILE=\"$(notdir $(basename $@))\" -o $@

$(TESTEROUT)/profiler: $(TESTEROBJ) $(TESTERLIB)
	@ mkdir -p $(dir $@)
	$(info Linking $(notdir $(basename $@)))
	@ $(CC) -g -std=c99 $(TESTEROBJ) $(TESTERLIB) -o $@

