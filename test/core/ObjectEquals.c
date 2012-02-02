#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	Bool equals;
	ClarityObject *parameters;
	ClarityObject *object1;
	ClarityObject *object2;

	object1 = clarityObjectCreate(heap);
	object2 = clarityObjectCreate(heap);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object1);
	equals = clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters));
	assert(equals);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object2);
	equals = clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters));
	assert(!equals);
}

