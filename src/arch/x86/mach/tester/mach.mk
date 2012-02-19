ifeq ($(shell uname),Darwin)
LD := gcc-apple-4.2
CC := gcc-apple-4.2
endif

COMPONENTS += $(notdir $(shell find $(COMPONENTSDIR) -type d -depth 1))
