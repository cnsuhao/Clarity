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

static Sint8 testStrCmp(ClarityCore *core, const char *cString,
	const char *cString2)
{
	return strcmp(cString, cString2);
}

static void entry(ClarityCore *core)
{
	const char *testStringBase = "BTestString";
	const char *testStringMore = "CTestString";
	const char *testStringLess = "ATestString";

	assert(clarityStrCmp(core, testStringBase, testStringBase) == 0);
	assert(clarityStrCmp(core, testStringBase, testStringMore) < 0);
	assert(clarityStrCmp(core, testStringBase, testStringLess) > 0);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	claritySetStrCmp(core, testStrCmp);
	clarityStart(core);
	clarityStop(core);
	return 0;
}
