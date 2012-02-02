#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include "ClarityFunctionObject.h"
#include <assert.h>

static ClarityObject *arrayObject;
static Bool gotCallback = FALSE;

static ClarityObject *mapFunction(ClarityObject *scope)
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
	return clarityIntegerObjectCreate(clarityHeap(scope), data * 2);
}

static ClarityObject *mapCallback(ClarityObject *scope)
{
	ClarityArray *array = clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1"));

	assert(clarityIntegerObjectGetValue(
		clarityArrayPop(array)) == 32);

	assert(clarityIntegerObjectGetValue(
		clarityArrayPop(array)) == 16);

	assert(clarityIntegerObjectGetValue(
		clarityArrayPop(array)) == 8);

	assert(clarityIntegerObjectGetValue(
		clarityArrayPop(array)) == 4);

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
		clarityFunctionObjectCreate(heap, mapFunction,
		NULL));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, mapCallback,
		NULL));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "map"), parameters);
}
