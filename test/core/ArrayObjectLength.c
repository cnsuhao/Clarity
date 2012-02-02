#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	Uint32 length;
	ClarityObject *parameters;
	ClarityObject *object;
	ClarityArray *array;

	array = clarityArrayCreate(heap);
	clarityArrayPush(array, clarityIntegerObjectCreate(heap, 2));
	clarityArrayPush(array, clarityIntegerObjectCreate(heap, 4));
	clarityArrayPush(array, clarityIntegerObjectCreate(heap, 8));
	object = clarityArrayObjectCreate(heap, array);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", object);
	length = clarityIntegerObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object, "length"), parameters));
	assert(3 == length);
}

