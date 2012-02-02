#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include <assert.h>

#define TEST_VALUE "test string"

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	Uint32 length;
	ClarityObject *parameters;
	ClarityObject *object;

	object = clarityStringObjectCreate(heap, TEST_VALUE);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", object);
	length = clarityIntegerObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object, "length"), parameters));
	assert(clarityStrLen(TEST_VALUE) == length);
}

