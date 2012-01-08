#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

#define TEST_VALUE "test string"

static ClarityObject *testFunction(ClarityObject *scope)
{
	return scope;
}

static void entry(ClarityCore *core)
{
	ClarityObject *parameters;
	ClarityObject *string;
	ClarityObject *function;
	ClarityObject *integer;
	ClarityObject *array;
	ClarityObject *boolean;
	ClarityObject *global = clarityGlobal(core);

	parameters = clarityObjectCreate(core);
	string = clarityStringObjectCreate(core, "test");
	clarityObjectSetMember(parameters, "$0", global);
	clarityObjectSetMember(parameters, "$1", string);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(global, "typeOf"),
		parameters))), "string") == 0);

	parameters = clarityObjectCreate(core);
	integer = clarityIntegerObjectCreate(core, 23);
	clarityObjectSetMember(parameters, "$0", global);
	clarityObjectSetMember(parameters, "$1", integer);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(global, "typeOf"),
		parameters))), "number") == 0);

	parameters = clarityObjectCreate(core);
	function = clarityFunctionObjectCreate(core, testFunction, global);
	clarityObjectSetMember(parameters, "$0", global);
	clarityObjectSetMember(parameters, "$1", function);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(global, "typeOf"),
		parameters))), "function") == 0);

	parameters = clarityObjectCreate(core);
	boolean = clarityBooleanObjectCreate(core, TRUE);
	clarityObjectSetMember(parameters, "$0", global);
	clarityObjectSetMember(parameters, "$1", boolean);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(clarityFunctionObjectCall(
		clarityObjectGetMember(global, "typeOf"),
		parameters))), "boolean") == 0);

	parameters = clarityObjectCreate(core);
	array = clarityArrayObjectCreate(core, clarityArrayCreate(core));
	clarityObjectSetMember(parameters, "$0", global);
	clarityObjectSetMember(parameters, "$1", array);
	assert(clarityStrCmp(core, clarityStringGetValue(
		(ClarityString *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(global, "typeOf"),
		parameters))), "array") == 0);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(core);
	clarityStop(core);
	return 0;
}
