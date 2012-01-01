#include "Clarity.h"
#include "ClarityHeap.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

static Uint32 testStrLen(Clarity *clarity, const char *cString)
{
	return strlen(cString);
}

static void entry(Clarity *clarity)
{
	const char *buffer = "TestString";

	assert(clarityStrLen(clarity, buffer) == 10);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate((ClarityEvent)entry, heap);
	claritySetStrLen(clarity, testStrLen);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
