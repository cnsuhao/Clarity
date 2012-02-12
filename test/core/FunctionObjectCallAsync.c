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
	ClarityObject *parameters;

	scope = clarityObjectCreate(heap);
	parameters = clarityObjectCreate(heap);
	object = clarityFunctionObjectCreateAsync(heap, testFunction, scope);
	clarityFunctionObjectCall(object, parameters);
	assert(clarityObjectIsTypeOf(
		clarityFunctionObjectCall(NULL, parameters), "undefined"));
	assert(clarityObjectIsTypeOf(
		clarityFunctionObjectCall(object, NULL), "undefined"));
}

