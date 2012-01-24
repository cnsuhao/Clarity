ifeq ($(ARCH), )
ARCH := x86
endif

ifeq ($(LINK), )
LINK := false
endif

ifeq ($(MACH), )
MACH := default
endif

ifeq ($(TARGET), )
TARGET := debug
endif

ifeq ($(OUT), )
OUT := out/rel/$(ARCH)-$(MACH)-$(TARGET)
endif
reverse = $(if $(1), \
	$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

OUTDIR := out/int/arch/$(ARCH)-$(MACH)-$(TARGET)
ANALYSISDIR := out/int/ana
DEPENDENCYDIR := out/int/dep
CLADIR := out/int/cla
CLAGENDIR := out/int/cla/gen
SRCDIR := src

export CLAOUT := $(abspath out/int/tools/compiler)
export CLA := $(abspath $(CLAOUT)/clarity)

COMPONENTS :=

AR := ar
AS := as
LD := gcc
CC := gcc
CPP := $(CC) -E
ARFLAGS := rsc
ASFLAGS :=
ifeq ($(CCCC), true)
CCCCCFLAG := -fdump-rtl-expand
endif
BASECFLAGS := -c -Wall -Werror -pedantic -std=c99 $(CCCCCFLAG)
BASELDFLAGS :=
ifeq ($(TARGET), release)
LDFLAGS := $(BASELDFLAGS)
CFLAGS := $(BASECFLAGS) -O2
else ifeq ($(TARGET), debug)
LDFLAGS := $(BASELDFLAGS) -g
CFLAGS := $(BASECFLAGS) -g
else ifeq ($(TARGET), coverage)
LDCOVERAGE := -lgcov
CCOVERAGE := -fprofile-arcs -ftest-coverage
LDFLAGS := -g $(LDCOVERAGE)
CFLAGS := $(BASECFLAGS) -g $(CCOVERAGE)
endif

-include src/arch/$(ARCH)/mach/$(MACH)/mach.mk
-include src/arch/$(ARCH)/arch.mk

ifeq ($(CROSS_COMPILE), )
CROSS_COMPILE :=
endif

INCLUDE := \
	src/include/ \
	src/arch/$(ARCH)/mach/$(MACH)/include/ \
	src/arch/$(ARCH)/include/ \
	src/arch/include/

SSOURCE := $(wildcard \
	src/arch/$(ARCH)/*.s \
	src/arch/$(ARCH)/mach/$(MACH)/*.s)

CSOURCE := $(wildcard \
	src/core/*.c \
	src/arch/$(ARCH)/*.c \
	src/arch/$(ARCH)/mach/$(MACH)/*.c)

COBJECTS := \
	$(addprefix $(OUTDIR)/, \
	$(patsubst %.c,%.o, $(CSOURCE)))

SOBJECTS := \
	$(addprefix $(OUTDIR)/, \
	$(patsubst %.s,%.o, $(SSOURCE)))

HEADER := $(wildcard \
	src/include/*.h \
	src/arch/include/*.h \
	src/arch/$(ARCH)/include/*.h \
	src/arch/$(ARCH)/mach/$(MACH)/include/*.h)

CLASOURCE := $(wildcard \
	src/core/*.cla \
	src/arch/$(ARCH)/*.cla \
	src/arch/$(ARCH)/mach/$(MACH)/*.cla \
	$(addsuffix /*.cla, $(addprefix src/components/, $(COMPONENTS))) \
	$(addsuffix /include/*.cla, $(addprefix src/components/, $(COMPONENTS))))

CLACSOURCE := \
	$(addprefix $(CLAGENDIR)/, \
	$(patsubst %.cla,%.c, $(CLASOURCE)))

CLAOBJECTS := \
	$(addprefix $(OUTDIR)/, \
	$(patsubst %.cla,%.o, $(CLASOURCE)))

CANALYSIS := \
	$(addprefix $(ANALYSISDIR)/, \
	$(patsubst %.c,%.c.cp, $(CSOURCE)))

HANALYSIS := \
	$(addprefix $(ANALYSISDIR)/, \
	$(patsubst %.h,%.h.cp, $(HEADER)))

ifeq ($(LINK), true)
$(OUT)/libclaritycore.a: $(OUTDIR)/claritycore
endif
ifeq ($(CCCC), true)
$(OUT)/libclaritycore.a: out/rel/report/cccc
endif
ifeq ($(EGYPT), true)
$(OUT)/libclaritycore.a: out/rel/report/callgraph.png
endif
$(OUT)/libclaritycore.a: $(CLA)
$(OUT)/libclaritycore.a: $(OUTDIR)/libclaritycore.a
	$(info Copying $@)
	@ mkdir -p $(dir $@)
	@ mkdir -p $(dir $@)/include
	@ cp $< $@
ifeq ($(LINK), true)
	@ cp $(OUTDIR)/claritycore $(OUT)/claritycore
	@ cp $(OUTDIR)/claritycore.map $(OUT)/claritycore.map
endif
	@ cp $(CLA) $(OUT)/
	@ for dir in $(call reverse,$(INCLUDE)) ; do \
	if [ -e $$dir ] ; then \
		cp -R $$dir*.h $(OUT)/include ; \
	fi ; \
	done

out/rel/report/cccc:
	@ mkdir -p $@
	@ cccc --outdir=$@ $(CSOURCE)

out/rel/report/callgraph.png: out/rel/report/callgraph.dot
	@ dot -Tpng -o $@ $<

out/rel/report/callgraph.dot: $(SOBJECTS) $(COBJECTS) $(CLAOBJECTS)
	@ egypt *.expand > $@
	@ rm -f *.expand

$(CLAGENDIR)/%.c: %.cla $(CLA)
	@ mkdir -p $(dir $@)
	$(info Generating $< implementation)
	@ $(CLA) -c -o $@ $<

$(OUTDIR)/%.o: $(CLAGENDIR)/%.c
	@ mkdir -p $(dir $@)
	$(info Compiling $<)
	@ $(CROSS_COMPILE)$(CC) $(addprefix -I, $(INCLUDE)) \
		-DCLARITY_FILE=\"$(notdir $(basename $@))\" \
		$(CFLAGS) -o $@ $(abspath $<)

$(OUTDIR)/%.o: %.c
	@ mkdir -p $(dir $@)
	$(info Compiling $<)
	@ $(CROSS_COMPILE)$(CC) $(addprefix -I, $(INCLUDE)) \
		-DCLARITY_FILE=\"$(notdir $(basename $@))\" \
		$(CFLAGS) -o $@ $(abspath $<)

$(OUTDIR)/%.o: %.s
	@ mkdir -p $(dir $@)
	$(info Compiling $<)
	@ $(CROSS_COMPILE)$(AS) $(ASFLAGS) -o $@ $(abspath $<)

$(DEPENDENCYDIR)/%.d: %.c
	@ mkdir -p $(dir $@)
	$(info Dependencies $<)
	@ $(CROSS_COMPILE)$(CC) $(addprefix -I, $(INCLUDE)) $(CFLAGS) -MM $< | \
		sed 's,\($(notdir $*)\)\.o[ :]*,$(OUTDIR)/\1.o $@ : ,g' > $@;

$(OUTDIR)/libclaritycore.a: $(HANALYSIS) $(CANALYSIS)
$(OUTDIR)/libclaritycore.a: $(SOBJECTS) $(COBJECTS) $(CLAOBJECTS)
	@ mkdir -p $(dir $@)
	$(info Archiving $@)
	@ $(CROSS_COMPILE)$(AR) $(ARFLAGS) $@ \
		$(SOBJECTS) $(COBJECTS) $(CLAOBJECTS)

$(ANALYSISDIR)/%.c.cp: %.c
	@ mkdir -p $(dir $@)
	$(info Analyzing $<)
	@ $(CHECKPATCH) $<
	@ touch $@

$(ANALYSISDIR)/%.h.cp: %.h
	@ mkdir -p $(dir $@)
	$(info Analyzing $<)
	@ $(CHECKPATCH) $<
	@ touch $@

$(OUTDIR)/$(ARCH)-$(MACH)-$(TARGET).cp: $(HEADER)
	$(info Analyzing $<)
	@ $(CHECKPATCH) $(HEADER)
	@ touch $@

$(OUTDIR)/claritycore: $(OUTDIR)/libclaritycore.a
	@ mkdir -p $(dir $@)
	$(info Linking $@)
	@ $(CROSS_COMPILE)$(LD) $(LDFLAGS) $< -o $@

-include $(addprefix $(DEPENDENCYDIR)/, $(CSOURCE:.c=.d))

$(CLA):
	@ $(MAKE) -C src/tools/compiler
