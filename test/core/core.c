#include "Clarity.h"
#include "ClarityString.h"
#include "ClarityObject.h"
#include "ClarityMacro.h">
#include <stdio.h>

static Uint32 count = 1;

static void printString(Clarity *clarity, void *data)
{
	count++;
	UNUSED(data);

	if (count < 1000) {
		clarityEnqueueEvent(clarity,
							printString,
							clarityStringCreate(clarity, "printA"));

		clarityEnqueueEvent(clarity,
							printString,
							clarityStringCreate(clarity, "printB"));

		clarityEnqueueEvent(clarity,
							printString,
							clarityStringCreate(clarity, "printC"));
	}
}

static void entry(Clarity *clarity, void *data)
{
	UNUSED(data);
	clarityEnqueueEvent(clarity,
						printString,
						clarityStringCreate(clarity, "entry"));
}

int main(void)
{
	clarityMain(entry);
	return 0;
}
