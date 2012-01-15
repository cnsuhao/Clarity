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

static ClarityObject *testFunction(ClarityObject *context)
{
	return clarityUndefined();
}

static void entry(ClarityCore *core)
{
	ClarityObject *object;
	ClarityObject *scope;

	scope = clarityObjectCreate(core);
	object = clarityFunctionObjectCreate(core, testFunction, scope);
	assert(clarityFunctionObjectCall(NULL, scope) == clarityUndefined());
	assert(clarityFunctionObjectCall(object, NULL) == clarityUndefined());
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
