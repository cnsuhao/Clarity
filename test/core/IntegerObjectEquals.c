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
	ClarityObject *object3;
	ClarityObject *object4;

	object1 = clarityIntegerObjectCreate(heap, 34);
	object2 = clarityIntegerObjectCreate(heap, 34);
	object3 = clarityIntegerObjectCreate(heap, 54);
	object4 = clarityBooleanObjectCreate(heap, TRUE);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object2);
	equals = clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters));
	assert(equals);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object3);
	equals = clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters));
	assert(!equals);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object4);
	equals = clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters));
	assert(!equals);
}

