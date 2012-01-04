#include "Clarity.h"
#include "ClarityHeap.h"
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

static void entry(ClarityCore *core)
{
	const char *testString = "TestString";
	const char *cString;
	ClarityString *string;

	string = clarityStringCreate(core, testString);
	cString = clarityStringGetCString(string);

	assert(strcmp(testString, cString) == 0);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(core);
	clarityStop(core);
	return 0;
}
