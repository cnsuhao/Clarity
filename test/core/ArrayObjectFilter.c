#include "Clarity.h"
#include <assert.h>

static ClarityObject *arrayObject;
static Bool gotCallback = FALSE;

static ClarityObject *filterFunction(ClarityObject *scope)
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
	return clarityBooleanObjectCreate(clarityHeap(scope), index != 2);
}

static ClarityObject *filterCallback(ClarityObject *scope)
{
	ClarityArray *array = clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1"));

	assert(clarityIntegerObjectGetValue(
		clarityArrayPop(array)) == 16);

	assert(clarityIntegerObjectGetValue(
		clarityArrayPop(array)) == 4);

	assert(clarityIntegerObjectGetValue(
		clarityArrayPop(array)) == 2);

	assert(clarityArrayPop(array) == NULL);

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
		clarityFunctionObjectCreate(heap, filterFunction,
		NULL));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, filterCallback,
		NULL));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "filter"), parameters);
}
