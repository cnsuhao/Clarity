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

	object1 = clarityBooleanObjectCreate(heap, TRUE);
	object2 = clarityBooleanObjectCreate(heap, TRUE);
	object3 = clarityBooleanObjectCreate(heap, FALSE);
	object4 = clarityStringObjectCreate(heap, "testString");
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

