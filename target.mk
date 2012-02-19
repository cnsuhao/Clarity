ifeq ($(PROJECT), )
PROJECT := project
endif

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

ifeq ($(SRCDIR), )
SRCDIR := src
endif

COREDIR := $(SRCDIR)/core
BASEDIR := $(SRCDIR)/arch
ARCHDIR := $(SRCDIR)/arch/$(ARCH)
MACHDIR := $(ARCHDIR)/mach/$(MACH)
COMPONENTSDIR := $(SRCDIR)/components

reverse = $(if $(1), \
	$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

OUTDIR := out
INTDIR := $(OUTDIR)/int
OBJDIR := $(INTDIR)/arch/$(ARCH)-$(MACH)-$(TARGET)
ANALYSISDIR := $(INTDIR)/ana
DEPENDENCYDIR := $(INTDIR)/dep
CLADIR := $(INTDIR)/cla
CLAGENDIR := $(INTDIR)/cla/gen
MAPFILE := $(OBJDIR)/$(PROJECT).map

export CLAOUT := $(abspath $(INTDIR)/tools/compiler)
export CLA := $(abspath $(CLAOUT)/clarity)

COMPONENTS :=

AR := ar
AS := as
LD := gcc
CC := gcc
CPP := $(CC) -E
ARFLAGS := rsc
ASFLAGS :=
ifeq ($(ARQUA), true)
ARQUACFLAG := -fdump-rtl-expand
endif
BASECFLAGS := -c -Wall -Werror -pedantic -std=c99 $(ARQUACFLAG)
BASELDFLAGS :=
ifeq ($(TARGET), release)
LDFLAGS := $(BASELDFLAGS)
CFLAGS := $(BASECFLAGS) -O2
else ifeq ($(TARGET), debug)
LDFLAGS := $(BASELDFLAGS) -g
CFLAGS := $(BASECFLAGS) -g
else ifeq ($(TARGET), coverage)
LDCOVERAGE := -coverage
CCOVERAGE := -coverage
LDFLAGS := -g $(LDCOVERAGE)
CFLAGS := $(BASECFLAGS) -g $(CCOVERAGE)
endif

-include $(MACHDIR)/mach.mk
-include $(ARCHDIR)/arch.mk

ifeq ($(CROSS_COMPILE), )
CROSS_COMPILE :=
endif

INCLUDE := \
	$(SRCDIR)/include/ \
	$(MACHDIR)/include/ \
	$(ARCHDIR)/include/ \
	$(BASEDIR)/include/ \
	$(COREDIR)/include/

SSOURCE := \
	$(shell find $(MACHDIR) -type f -name '*.s')

CSOURCE := \
	$(shell find $(COREDIR) -type f -name '*.c') \
	$(shell find $(MACHDIR) -type f -name '*.c')

COBJECTS := \
	$(addprefix $(OBJDIR)/, \
	$(patsubst %.c,%.o, $(CSOURCE)))

CEXPANDS := \
	$(patsubst %.o,%.expand, $(COBJECTS))

CDOTS := \
	$(patsubst %.expand,%.dot, $(CEXPANDS))

CPDFS := \
	$(patsubst %.dot,%.pdf, $(CDOTS))

SOBJECTS := \
	$(addprefix $(OBJDIR)/, \
	$(patsubst %.s,%.o, $(SSOURCE)))

HEADER := $(wildcard \
	$(SRCDIR)/include/*.h \
	$(BASEDIR)/include/*.h \
	$(ARCHDIR)/include/*.h \
	$(MACHDIR)/include/*.h)

CLASOURCE := \
	$(shell find $(ARCHDIR) -type f -name '*.cla') \
	$(shell find $(MACHDIR) -type f -name '*.cla') \
	$(wildcard $(addsuffix /*.cla, $(addprefix $(COMPONENTSDIR)/, $(COMPONENTS))) \
	$(addsuffix /include/*.cla, $(addprefix $(COMPONENTSDIR)/, $(COMPONENTS))))

CLACSOURCE := \
	$(addprefix $(CLAGENDIR)/, \
	$(patsubst %.cla,%.c, $(CLASOURCE)))

CLAOBJECTS := \
	$(addprefix $(OBJDIR)/, \
	$(patsubst %.cla,%.o, $(CLASOURCE)))

CANALYSIS := \
	$(addprefix $(ANALYSISDIR)/, \
	$(patsubst %.c,%.c.cp, $(CSOURCE)))

HANALYSIS := \
	$(addprefix $(ANALYSISDIR)/, \
	$(patsubst %.h,%.h.cp, $(HEADER)))

ifeq ($(LINK), true)
$(OUT)/lib$(PROJECT).a: $(OBJDIR)/$(PROJECT)
endif
ifeq ($(CCCC), true)
$(OUT)/lib$(PROJECT).a: out/rel/report/cccc
endif
ifeq ($(ARQUA), true)
$(OUT)/lib$(PROJECT).a: out/rel/report/archreport.pdf
$(OUT)/lib$(PROJECT).a: $(CDOTS)
endif
$(OUT)/lib$(PROJECT).a: $(CLA)
$(OUT)/lib$(PROJECT).a: $(OBJDIR)/lib$(PROJECT).a
	$(info Copying $@)
	@ mkdir -p $(dir $@)
	@ mkdir -p $(dir $@)/include
	@ cp $< $@
ifeq ($(LINK), true)
	@ cp $(OBJDIR)/$(PROJECT) $(OUT)/$(PROJECT)
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

out/rel/report/archreport.pdf: $(OBJDIR)/archreport.pdf
	@ mkdir -p $(dir $@)
	@ cp $< $@

$(OBJDIR)/archreport.pdf: $(CPDFS) $(OBJDIR)/system.pdf
	@ pdfunite $(OBJDIR)/system.pdf $(CPDFS) $@

%.pdf: %.dot
	@ unflatten -c4 $< | dot -Tpdf -Gfontsize=14 -Gratio="0.681" -Gsize="8.27,11.69 -Glandscape" -o $@

$(OBJDIR)/system.dot : $(COBJECTS)
	@ arqua --root $(OBJDIR)/$(SRCDIR)/core --stop 2 -functions $(CEXPANDS) > $@

$(OBJDIR)/%.dot: $(OBJDIR)/%.expand
	@ arqua --root $(OBJDIR)/$(SRCDIR)/core --start 0 --stop 1 $< > $@

$(CLAGENDIR)/%.c: %.cla $(CLA)
	@ mkdir -p $(dir $@)
	$(info Generating $<)
	@ $(CLA) -c -o $@ $<

$(OBJDIR)/%.o: $(INCLUDE)
$(OBJDIR)/%.o: $(CLAGENDIR)/%.c
	@ mkdir -p $(dir $@)
	$(info Compiling $<)
	@ $(CROSS_COMPILE)$(CC) $(addprefix -I, $(INCLUDE)) \
		-DCLARITY_FILE=\"$(notdir $(basename $@))\" \
		$(CFLAGS) -o $@ $(abspath $<)

$(OBJDIR)/%.o: %.c
	@ mkdir -p $(dir $@)
	$(info Compiling $<)
	@ $(CROSS_COMPILE)$(CC) $(addprefix -I, $(INCLUDE)) \
		-DCLARITY_FILE=\"$(notdir $(basename $@))\" \
		$(CFLAGS) -o $@ $(abspath $<)
ifeq ($(ARQUA), true)
	@ mv $(notdir $<).104r.expand $(dir $@)/$(notdir $(basename $@)).expand
endif

$(OBJDIR)/%.o: %.s
	@ mkdir -p $(dir $@)
	$(info Compiling $<)
	@ $(CROSS_COMPILE)$(AS) $(ASFLAGS) -o $@ $(abspath $<)

$(DEPENDENCYDIR)/%.d: %.c
	@ mkdir -p $(dir $@)
	$(info Dependencies $<)
	@ $(CROSS_COMPILE)$(CC) $(addprefix -I, $(INCLUDE)) $(CFLAGS) -MM $< | \
		sed 's,\($(notdir $*)\)\.o[ :]*,$(OBJDIR)/\1.o $@ : ,g' > $@;

$(OBJDIR)/lib$(PROJECT).a: $(HANALYSIS) $(CANALYSIS)
$(OBJDIR)/lib$(PROJECT).a: $(SOBJECTS) $(COBJECTS) $(CLAOBJECTS)
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

$(OBJDIR)/$(ARCH)-$(MACH)-$(TARGET).cp: $(HEADER)
	$(info Analyzing $<)
	@ $(CHECKPATCH) $(HEADER)
	@ touch $@

$(OBJDIR)/$(PROJECT): $(OBJDIR)/lib$(PROJECT).a
	@ mkdir -p $(dir $@)
	$(info Linking $@)
	@ $(CROSS_COMPILE)$(LD) $(LDFLAGS) $< -o $@

-include $(addprefix $(DEPENDENCYDIR)/, $(CSOURCE:.c=.d))

$(CLA):
	@ $(MAKE) -C $(SRCDIR)/tools/compiler
