#include "Clarity.h"
#include "ClarityString.h"
#include "stdio.h"

static int count = 1000;

static void printString(Clarity *clarity, void *data)
{
	printf("The String: %s %d\n",
		  clarityStringGetCString((ClarityString *)data), count);

	if (count > 0) {
		count--;
		clarityEnqueueEvent(clarity,
							printString,
							clarityStringCreate(clarity, "print"));
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
	clarityStart(clarityCreate(entry));
	return 0;
}
