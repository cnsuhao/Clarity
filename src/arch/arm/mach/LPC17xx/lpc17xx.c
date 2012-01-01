#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityObject.h"
#include "ClarityInteger.h"
#include <stdlib.h>

static void entry(Clarity *clarity)
{
	clarityObjectCreate(clarity);
	clarityIntegerCreate(clarity, 32);
}

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

void start(void)
{
	Clarity *clarity;
	ClarityHeap *heap;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
}
