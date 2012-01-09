#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
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

static void entry(ClarityCore *core)
{
	Bool equals;
	ClarityObject *parameters;
	ClarityObject *object1;
	ClarityObject *object2;
	ClarityObject *object3;

	object1 = clarityIntegerObjectCreate(core, 34);
	object2 = clarityIntegerObjectCreate(core, 34);
	object3 = clarityIntegerObjectCreate(core, 54);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "$0", object1);
	clarityObjectSetMember(parameters, "$1", object2);
	equals = clarityBooleanGetValue((ClarityBoolean *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters)));
	assert(equals);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "$0", object1);
	clarityObjectSetMember(parameters, "$1", object3);
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
