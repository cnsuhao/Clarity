#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include <assert.h>

#define TEST_VALUE "test string"

static ClarityObject *testFunction(ClarityObject *scope)
{
	return scope;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *parameters;
	ClarityObject *string;
	ClarityObject *function;
	ClarityObject *integer;
	ClarityObject *array;
	ClarityObject *boolean;

	parameters = clarityObjectCreate(core);
	string = clarityStringObjectCreate(core, "test");
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", string);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(globalScope, "typeOf"),
		parameters))), "string") == 0);

	parameters = clarityObjectCreate(core);
	integer = clarityIntegerObjectCreate(core, 23);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", integer);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(globalScope, "typeOf"),
		parameters))), "number") == 0);

	parameters = clarityObjectCreate(core);
	function = clarityFunctionObjectCreate(core, testFunction, globalScope);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", function);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(globalScope, "typeOf"),
		parameters))), "function") == 0);

	parameters = clarityObjectCreate(core);
	boolean = clarityBooleanObjectCreate(core, TRUE);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", boolean);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "typeOf"),
		parameters))), "boolean") == 0);

	parameters = clarityObjectCreate(core);
	array = clarityArrayObjectCreate(core, clarityArrayCreate(core));
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", array);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(globalScope, "typeOf"),
		parameters))), "array") == 0);
}

