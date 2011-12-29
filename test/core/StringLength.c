#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityMacro.h"
#include "ClarityString.h"
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

static void entry(Clarity *clarity, void *data)
{
	const char *testString = "TestString";
	ClarityString *string;

	UNUSED(data);
	string = clarityStringCreate(clarity, testString);
	assert(strlen(testString) == clarityStringLength(string));
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
