#include "Clarity.h"
#include "ClarityString.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int count = 1;

static void printString(Clarity *clarity, void *data)
{
	count++;
	printf("The String: %s %d\n",
		   clarityStringGetCString((ClarityString *)data), count);

	if (count < 20000) {
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
	ClarityString *string;

	UNUSED(data);
	string = clarityStringCreate(clarity, "string to auto release");

	if (clarityStringLength(string) == 22) {
		clarityEnqueueEvent(clarity,
							printString,
							clarityStringCreate(clarity, "entry"));
	}
}

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate(entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
