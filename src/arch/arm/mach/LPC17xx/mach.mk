LDFLAGS += -mthumb -T$(MACHDIR)/mach.ld -nostartfiles -Wl,-Map,$(MAPFILE)
CFLAGS += -mcpu=cortex-m3 -mthumb
ASFLAGS += -mcpu=cortex-m3 -mthumb
