ifeq ($(CLAOUT), )
CLAOUT := out
endif

ifeq ($(CLA), )
CLA := $(CLAOUT)/clarity
endif

$(CLA) :
	@ mkdir -p $(dir $@)
	@ cp clarity $(CLA)

clean :
	@ rm -rf $(CLAOUT)
