#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include "ClarityFunctionObject.h"
#include <assert.h>

static ClarityObject *parameters;
static ClarityObject *arrayObject;
static Bool gotCallback = FALSE;

static ClarityObject *forEachFunction(ClarityObject *scope)
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
	return NULL;
}

static ClarityObject *forEachCallback(ClarityObject *scope)
{
	assert(scope == parameters);
	gotCallback = TRUE;
	return NULL;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
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
		clarityFunctionObjectCreate(heap, forEachFunction,
		NULL));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, forEachCallback,
		NULL));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "forEach"), parameters);
}
