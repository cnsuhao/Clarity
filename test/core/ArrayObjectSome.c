#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include "ClarityFunctionObject.h"
#include <assert.h>

static ClarityObject *arrayObject;
static Bool gotCallback = FALSE;

static ClarityObject *someFunction1(ClarityObject *scope)
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

static ClarityObject *someCallback1(ClarityObject *scope)
{
	assert(clarityBooleanObjectGetValue((
		clarityObjectGetOwnMember(scope, "$1"))) == TRUE);
	gotCallback = TRUE;
	return NULL;
}

static ClarityObject *someFunction2(ClarityObject *scope)
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
	return clarityBooleanObjectCreate(clarityHeap(scope), FALSE);
}

static ClarityObject *someCallback2(ClarityObject *scope)
{
	assert(clarityBooleanObjectGetValue((
		clarityObjectGetOwnMember(scope, "$1"))) == FALSE);
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
		clarityFunctionObjectCreate(heap, someFunction1,
		NULL));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, someCallback1,
		NULL));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "some"), parameters);

	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(heap, someFunction2,
		NULL));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, someCallback2,
		NULL));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "some"), parameters);
}
