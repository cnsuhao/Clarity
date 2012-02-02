#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

static ClarityObject *testFunction(ClarityObject *scope)
{
	ClarityObject *this = clarityObjectGetMember(scope, "this");

	clarityObjectSetMember(this, "testMember",
		clarityObjectGetOwnMember(scope, "$1"));
	return NULL;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *function;
	ClarityObject *scope;
	ClarityObject *newObject;

	scope = clarityObjectCreate(heap);
	function = clarityFunctionObjectCreate(heap, testFunction, scope);
	newObject = clarityFunctionObjectNew(function,
		clarityObjectSetMember(clarityObjectCreate(heap), "$1",
		clarityIntegerObjectCreate(heap, 34)));
	assert(clarityIntegerObjectGetValue(
		clarityObjectGetMember(newObject, "testMember")) == 34);
}

