#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	Uint32 length;
	ClarityObject *parameters;
	ClarityObject *object;
	ClarityArray *array;

	array = clarityArrayCreate(core);
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 2));
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 4));
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 8));
	object = clarityArrayObjectCreate(core, array);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", object);
	length = clarityIntegerGetValue(
		(ClarityInteger *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object, "length"), parameters)));
	assert(3 == length);
}

