#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityNumberObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityCore.h"
#include <assert.h>

#define TEST_VALUE "test string"

static ClarityObject *testFunction(ClarityObject *scope)
{
	return scope;
}

static ClarityObject *clarityEntry(ClarityObject *globalScope)
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
	integer = clarityNumberObjectCreate(heap, 23);
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
	boolean = clarityBooleanObjectCreate(heap, 1);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", boolean);
	assert(clarityStrCmp(clarityStringObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "typeOf"),
		parameters)), "boolean") == 0);

	parameters = clarityObjectCreate(heap);
	array = clarityArrayObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", array);
	assert(clarityStrCmp(clarityStringObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "typeOf"),
		parameters)), "array") == 0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
