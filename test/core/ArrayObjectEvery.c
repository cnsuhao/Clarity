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

static ClarityObject *everyFunction1(ClarityObject *scope)
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
	return clarityBooleanObjectCreate(clarityCore(scope), (data == 4));
}

static ClarityObject *everyCallback1(ClarityObject *scope)
{
	assert(clarityBooleanGetValue(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$1"))) == FALSE);
	gotCallback = TRUE;
	return clarityUndefined();
}

static ClarityObject *everyFunction2(ClarityObject *scope)
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
	return clarityBooleanObjectCreate(clarityCore(scope), TRUE);
}

static ClarityObject *everyCallback2(ClarityObject *scope)
{
	assert(clarityBooleanGetValue(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$1"))) == TRUE);
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
		clarityFunctionObjectCreate(core, everyFunction1,
		clarityUndefined()));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, everyCallback1,
		clarityUndefined()));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "every"), parameters);

	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "$0", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(core, everyFunction2,
		clarityUndefined()));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, everyCallback2,
		clarityUndefined()));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "every"), parameters);
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
