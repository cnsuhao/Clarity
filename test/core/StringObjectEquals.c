#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityIntegerObject.h"
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

#define TEST_VALUE1 "test string"
#define TEST_VALUE2 "other test string"

static void entry(ClarityCore *core)
{
	Bool equals;
	ClarityObject *parameters;
	ClarityObject *object1;
	ClarityObject *object2;
	ClarityObject *object3;
	ClarityObject *object4;

	object1 = clarityStringObjectCreate(core, TEST_VALUE1);
	object2 = clarityStringObjectCreate(core, TEST_VALUE1);
	object3 = clarityStringObjectCreate(core, TEST_VALUE2);
	object4 = clarityIntegerObjectCreate(core, 23);
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
