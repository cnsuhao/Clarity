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
	const char *cStringLess = "ATestString";
	const char *cStringBase = "BTestString";
	const char *cStringMore = "CTestString";
	ClarityString *stringBase;
	ClarityString *stringSame;
	ClarityString *stringLess;
	ClarityString *stringMore;
	
	UNUSED(data);
	stringBase = clarityStringCreate(clarity, cStringBase);
	stringSame = clarityStringCreate(clarity, cStringBase);
	stringLess = clarityStringCreate(clarity, cStringLess);
	stringMore = clarityStringCreate(clarity, cStringMore);
	assert(clarityStringCompare(stringBase, stringSame) == 0);
	assert(clarityStringCompare(stringBase, stringLess) > 0);
	assert(clarityStringCompare(stringBase, stringMore) < 0);
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
