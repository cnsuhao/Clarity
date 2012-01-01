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

static void entry(Clarity *clarity)
{
	const char *testStringBase = "BTestString";
	const char *testStringMore = "CTestString";
	const char *testStringLess = "ATestString";

	assert(clarityStrCmp(clarity, testStringBase, testStringBase) == 0);
	assert(clarityStrCmp(clarity, testStringBase, testStringMore) < 0);
	assert(clarityStrCmp(clarity, testStringBase, testStringLess) > 0);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
