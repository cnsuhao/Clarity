#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	Bool equals;
	ClarityObject *parameters;
	ClarityObject *object1;
	ClarityObject *object2;

	object1 = clarityObjectCreate(core);
	object2 = clarityObjectCreate(core);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object1);
	equals = clarityBooleanGetValue((ClarityBoolean *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters)));
	assert(equals);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object2);
	equals = clarityBooleanGetValue((ClarityBoolean *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters)));
	assert(!equals);
}

