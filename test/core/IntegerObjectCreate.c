#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityIntegerObject.h"
#include "ClarityInteger.h"
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

#define TEST_VALUE 23

static void entry(ClarityCore *core)
{
	ClarityObject *object;
	ClarityInteger *integer;

	object = clarityIntegerObjectCreate(core, TEST_VALUE);
	integer = clarityObjectGetInnerData(object);
	assert(clarityIntegerGetValue(integer) == TEST_VALUE);
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
