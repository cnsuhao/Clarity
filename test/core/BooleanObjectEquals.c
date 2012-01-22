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
	ClarityObject *object3;
	ClarityObject *object4;

	object1 = clarityBooleanObjectCreate(core, TRUE);
	object2 = clarityBooleanObjectCreate(core, TRUE);
	object3 = clarityBooleanObjectCreate(core, FALSE);
	object4 = clarityStringObjectCreate(core, "testString");
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object2);
	equals = clarityBooleanGetValue((ClarityBoolean *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters)));
	assert(equals);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object3);
	equals = clarityBooleanGetValue((ClarityBoolean *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters)));
	assert(!equals);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", object1);
	clarityObjectSetMember(parameters, "$1", object4);
	equals = clarityBooleanGetValue((ClarityBoolean *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters)));
	assert(!equals);

}

