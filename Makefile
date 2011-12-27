.PHONY : main test build clean

main : test build

build :
	@ $(MAKE) -f target.mk

test :
	@ $(MAKE) -f tester.mk

clean :
	rm -rf out