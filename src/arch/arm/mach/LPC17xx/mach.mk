LDFLAGS += -mthumb -Tsrc/arch/$(ARCH)/mach/$(MACH)/mach.ld -nostartfiles -Wl,-Map,$(OUTDIR)/claritycore.map
CFLAGS += -mcpu=cortex-m3 -mthumb
ASFLAGS += -mcpu=cortex-m3 -mthumb