LDFLAGS += -mmcu=atmega328 -nostdlib -Wl,-Map,$(OUTDIR)/claritycore.map
CFLAGS += -mmcu=atmega328
ASFLAGS += -mmcu=atmega328