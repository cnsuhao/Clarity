#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
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

static ClarityObject *testFunction(ClarityObject *scope)
{
	ClarityObject *this = clarityObjectGetMember(scope, "this");

	clarityObjectSetMember(this, "testMember",
		clarityObjectGetOwnMember(scope, "$1"));
	return clarityUndefined();
}

static void entry(ClarityCore *core)
{
	ClarityObject *function;
	ClarityObject *scope;
	ClarityObject *functionCall;
	ClarityObject *newObject;

	scope = clarityObjectCreate(core);
	function = clarityFunctionObjectCreate(core, testFunction, scope);
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$1",
		clarityIntegerObjectCreate(core, 34));
	newObject = clarityFunctionObjectNew(function, functionCall);
	assert(clarityIntegerGetValue(
		clarityObjectGetInnerData(
		clarityObjectGetMember(newObject, "testMember"))) == 34);
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
