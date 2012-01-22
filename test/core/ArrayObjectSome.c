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

	data = clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1")));

	index = clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$2")));

	assert((data == 2 && index == 0) ||
		(data == 4 && index == 1) ||
		(data == 8 && index == 2) ||
		(data == 16 && index == 3));

	array = clarityObjectGetOwnMember(scope, "$3");
	assert(array == arrayObject);
	return clarityBooleanObjectCreate(clarityCore(), (data == 4));
}

static ClarityObject *someCallback1(ClarityObject *scope)
{
	assert(clarityBooleanGetValue(clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1"))) == TRUE);
	gotCallback = TRUE;
	return clarityUndefined();
}

static ClarityObject *someFunction2(ClarityObject *scope)
{
	Uint32 data;
	Uint32 index;
	ClarityObject *array;

	data = clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1")));

	index = clarityIntegerGetValue(clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$2")));

	assert((data == 2 && index == 0) ||
		(data == 4 && index == 1) ||
		(data == 8 && index == 2) ||
		(data == 16 && index == 3));

	array = clarityObjectGetOwnMember(scope, "$3");
	assert(array == arrayObject);
	return clarityBooleanObjectCreate(clarityCore(), FALSE);
}

static ClarityObject *someCallback2(ClarityObject *scope)
{
	assert(clarityBooleanGetValue(clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1"))) == FALSE);
	gotCallback = TRUE;
	return clarityUndefined();
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *parameters;
	ClarityArray *array;

	array = clarityArrayCreate(core);
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 2));
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 4));
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 8));
	clarityArrayPush(array, clarityIntegerObjectCreate(core, 16));
	arrayObject = clarityArrayObjectCreate(core, array);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(core, someFunction1,
		clarityUndefined()));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, someCallback1,
		clarityUndefined()));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "some"), parameters);

	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(core, someFunction2,
		clarityUndefined()));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, someCallback2,
		clarityUndefined()));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "some"), parameters);
}
