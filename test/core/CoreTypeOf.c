#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityCore.h"
#include <assert.h>

#define TEST_VALUE "test string"

static ClarityObject *testFunction(ClarityObject *scope)
{
	return scope;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;
	ClarityObject *string;
	ClarityObject *function;
	ClarityObject *integer;
	ClarityObject *array;
	ClarityObject *boolean;

	parameters = clarityObjectCreate(heap);
	string = clarityStringObjectCreate(heap, "test");
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", string);
	assert(clarityStrCmp(clarityStringObjectGetValue(
		clarityFunctionObjectCall(clarityObjectGetMember(globalScope,
		"typeOf"), parameters)), "string") == 0);

	parameters = clarityObjectCreate(heap);
	integer = clarityIntegerObjectCreate(heap, 23);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", integer);
	assert(clarityStrCmp(clarityStringObjectGetValue(
		clarityFunctionObjectCall(clarityObjectGetMember(globalScope,
		"typeOf"), parameters)), "number") == 0);

	parameters = clarityObjectCreate(heap);
	function = clarityFunctionObjectCreate(heap, testFunction, globalScope);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", function);
	assert(clarityStrCmp(clarityStringObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "typeOf"),
		parameters)), "function") == 0);

	parameters = clarityObjectCreate(heap);
	boolean = clarityBooleanObjectCreate(heap, TRUE);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", boolean);
	assert(clarityStrCmp(clarityStringObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "typeOf"),
		parameters)), "boolean") == 0);

	parameters = clarityObjectCreate(heap);
	array = clarityArrayObjectCreate(heap, clarityArrayCreate(heap));
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", array);
	assert(clarityStrCmp(clarityStringObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "typeOf"),
		parameters)), "array") == 0);
}

