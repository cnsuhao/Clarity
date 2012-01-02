#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityObject.h"
#include "ClarityInteger.h"
#include "ClarityStringObject.h"
#include "ClarityFunctionObject.h"
#include <stdlib.h>

static void *testFunction(void)
{
	return NULL;
}

static void entry(ClarityCore *core)
{
	clarityObjectCreate(core);
	clarityStringObjectCreate(core, "string");
	clarityFunctionObjectCreate(core,
		(ClarityFunctionPointer)testFunction);
	clarityIntegerCreate(core, 32);
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
	ClarityCore *clarity;
	ClarityHeap *heap;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
}
