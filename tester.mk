TESTEROUT := out/tester

main : build
	@ $(MAKE) -f target.mk

build : 
	@ $(MAKE) -f target.mk ARCH=x86 MACH=default OUT=$(TESTEROUT)

clean :
	@ $(MAKE) -f target.mk clean
