#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_VALUE "test string"

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	Uint32 length;
	ClarityObject *parameters;
	ClarityObject *object;

	object = clarityStringObjectCreate(core, TEST_VALUE);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", object);
	length = clarityIntegerGetValue((ClarityInteger *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object, "length"), parameters)));
	assert(clarityStrLen(core, TEST_VALUE) == length);
}

