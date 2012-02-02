#include "Clarity.h"
#include <assert.h>

static ClarityObject *arrayObject;
static Bool gotCallback = FALSE;

static ClarityObject *everyFunction1(ClarityObject *scope)
{
	Uint32 data;
	Uint32 index;
	ClarityObject *array;

	data = clarityIntegerObjectGetValue(
		clarityObjectGetOwnMember(scope, "$1"));

	index = clarityIntegerObjectGetValue(
		clarityObjectGetOwnMember(scope, "$2"));

	assert((data == 2 && index == 0) ||
		(data == 4 && index == 1) ||
		(data == 8 && index == 2) ||
		(data == 16 && index == 3));

	array = clarityObjectGetOwnMember(scope, "$3");
	assert(array == arrayObject);
	return clarityBooleanObjectCreate(clarityHeap(scope), (data == 4));
}

static ClarityObject *everyCallback1(ClarityObject *scope)
{
	assert(clarityBooleanObjectGetValue(
		clarityObjectGetOwnMember(scope, "$1")) == FALSE);
	gotCallback = TRUE;
	return NULL;
}

static ClarityObject *everyFunction2(ClarityObject *scope)
{
	Uint32 data;
	Uint32 index;
	ClarityObject *array;

	data = clarityIntegerObjectGetValue(
		clarityObjectGetOwnMember(scope, "$1"));

	index = clarityIntegerObjectGetValue(
		clarityObjectGetOwnMember(scope, "$2"));

	assert((data == 2 && index == 0) ||
		(data == 4 && index == 1) ||
		(data == 8 && index == 2) ||
		(data == 16 && index == 3));

	array = clarityObjectGetOwnMember(scope, "$3");
	assert(array == arrayObject);
	return clarityBooleanObjectCreate(clarityHeap(scope), TRUE);
}

static ClarityObject *everyCallback2(ClarityObject *scope)
{
	assert(clarityBooleanObjectGetValue(
		clarityObjectGetOwnMember(scope, "$1")) == TRUE);
	gotCallback = TRUE;
	return NULL;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;
	ClarityArray *array;

	array = clarityArrayCreate(heap);
	clarityArrayPush(array, clarityIntegerObjectCreate(heap, 2));
	clarityArrayPush(array, clarityIntegerObjectCreate(heap, 4));
	clarityArrayPush(array, clarityIntegerObjectCreate(heap, 8));
	clarityArrayPush(array, clarityIntegerObjectCreate(heap, 16));
	arrayObject = clarityArrayObjectCreate(heap, array);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(heap, everyFunction1,
		NULL));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, everyCallback1,
		NULL));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "every"), parameters);

	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(heap, everyFunction2,
		NULL));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, everyCallback2,
		NULL));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "every"), parameters);
}
