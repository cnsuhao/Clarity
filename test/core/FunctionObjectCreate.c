#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

static ClarityObject *testFunction(ClarityObject *context)
{
	return NULL;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;
	ClarityObject *context;

	context = clarityObjectCreate(heap);
	object = clarityFunctionObjectCreate(heap, testFunction, context);
	assert(object != NULL);
}

