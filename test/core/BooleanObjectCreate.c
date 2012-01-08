#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
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
	ClarityObject *object;
	ClarityBoolean *boolean;

	object = clarityBooleanObjectCreate(core, TRUE);
	assert(object != NULL);
	boolean = clarityObjectGetInnerData(object);
	assert(clarityBooleanGetValue(boolean) == TRUE);
	object = clarityBooleanObjectCreate(core, FALSE);
	assert(object != NULL);
	boolean = clarityObjectGetInnerData(object);
	assert(clarityBooleanGetValue(boolean) == FALSE);
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
