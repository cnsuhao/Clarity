main :
	@ $(MAKE) -f target.mk

test :
	@ $(MAKE) -f tester.mk

clean :
	@ echo Cleaning
	rm -rf out