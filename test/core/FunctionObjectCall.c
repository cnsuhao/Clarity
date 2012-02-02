#include "Clarity.h"
#include "ClarityCore.h"
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
	ClarityObject *scope;

	scope = clarityObjectCreate(heap);
	object = clarityFunctionObjectCreate(heap, testFunction, scope);
	assert(clarityStrCmp(clarityObjectTypeOf(
		clarityFunctionObjectCall(NULL, scope)), "undefined") == 0);
	assert(clarityStrCmp(clarityObjectTypeOf(
		clarityFunctionObjectCall(object, NULL)), "undefined") == 0);
}

