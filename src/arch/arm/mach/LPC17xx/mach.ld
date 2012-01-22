ENTRY(interruptVector)

MEMORY
{
	flash (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
	ram (rwx) : ORIGIN = 0x10000000, LENGTH = 32K
	ram1(rwx) : ORIGIN = 0x2007C000, LENGTH = 32k
}

SECTIONS {
	.flash : {
		interruptVector = ABSOLUTE(.);
		*(.interruptVector);
		vectorsLength = ABSOLUTE(.) - interruptVector;
		*(.text);
		*(.rodata);
		flashRamStart = ABSOLUTE(.);
	} >flash

	.ram : {
		ramStart = ABSOLUTE(.);
		*(.data);
		*(.bss);
		*(COMMON);
		ramEnd = ABSOLUTE(.);
		. = . + 1024;
		stack = ABSOLUTE(.);
		end = .;
	} >ram AT>flash

	ramSize = ramEnd - ramStart;
}
