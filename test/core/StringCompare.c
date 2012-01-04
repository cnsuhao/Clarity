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
	const char *cStringLess = "ATestString";
	const char *cStringBase = "BTestString";
	const char *cStringMore = "CTestString";
	ClarityString *stringBase;
	ClarityString *stringSame;
	ClarityString *stringLess;
	ClarityString *stringMore;

	stringBase = clarityStringCreate(core, cStringBase);
	stringSame = clarityStringCreate(core, cStringBase);
	stringLess = clarityStringCreate(core, cStringLess);
	stringMore = clarityStringCreate(core, cStringMore);
	assert(clarityStringCompare(stringBase, stringSame) == 0);
	assert(clarityStringCompare(stringBase, stringLess) > 0);
	assert(clarityStringCompare(stringBase, stringMore) < 0);
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
