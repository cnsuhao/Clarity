#include "Clarity.h"
#include "ClarityString.h"
#include "stdio.h"

static int count = 0;

static void printString(Clarity *clarity, void *data)
{
	count++;
	printf("The String: %s %d\n",
		   clarityStringGetCString((ClarityString *)data), count);

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
	Clarity *clarity;

	clarity = clarityCreate(entry);
	clarityStart(clarity);
	return 0;
}
