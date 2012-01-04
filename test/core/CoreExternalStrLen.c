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

static Uint32 testStrLen(ClarityCore *core, const char *cString)
{
	return strlen(cString);
}

static void entry(ClarityCore *core)
{
	const char *buffer = "TestString";

	assert(clarityStrLen(core, buffer) == 10);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	claritySetStrLen(core, testStrLen);
	clarityStart(core);
	clarityStop(core);
	return 0;
}
