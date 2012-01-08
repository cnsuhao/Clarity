#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include "ClarityFunctionObject.h"
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

static ClarityObject *arrayObject;
static Bool gotCallback = FALSE;

static ClarityObject *mapFunction(ClarityObject *scope)
{
	Uint32 data;
	Uint32 index;
	ClarityObject *array;

	data = clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$1")));

	index = clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$2")));

	assert((data == 2 && index == 0) ||
		(data == 4 && index == 1) ||
		(data == 8 && index == 2) ||
		(data == 16 && index == 3));

	array = clarityObjectGetMember(scope, "$3");
	assert(array == arrayObject);
	return clarityIntegerObjectCreate(clarityCore(scope), data * 2);
}

static ClarityObject *mapCallback(ClarityObject *scope)
{
	ClarityArray *array = clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$1"));

	assert(clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityArrayPop(array))) == 32);

	assert(clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityArrayPop(array))) == 16);

	assert(clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityArrayPop(array))) == 8);

	assert(clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityArrayPop(array))) == 4);

	gotCallback = TRUE;
	return clarityUndefined();
}

static void entry(ClarityCore *core)
{
	ClarityObject *parameters;
	ClarityArray *array;

	array = clarityArrayCreate(core);
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 2));
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 4));
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 8));
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 16));
	arrayObject = clarityArrayObjectCreate(core, array);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "$0", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(core, mapFunction,
		clarityUndefined()));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, mapCallback,
		clarityUndefined()));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "map"), parameters);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(core);
	clarityStop(core);
	assert(gotCallback);
	return 0;
}
