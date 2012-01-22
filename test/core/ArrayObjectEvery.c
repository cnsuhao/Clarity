#include "Clarity.h"
#include <assert.h>

static ClarityObject *arrayObject;
static Bool gotCallback = FALSE;

static ClarityObject *everyFunction1(ClarityObject *scope)
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

static ClarityObject *everyCallback1(ClarityObject *scope)
{
	assert(clarityBooleanGetValue(clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1"))) == FALSE);
	gotCallback = TRUE;
	return clarityUndefined();
}

static ClarityObject *everyFunction2(ClarityObject *scope)
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
	return clarityBooleanObjectCreate(clarityCore(), TRUE);
}

static ClarityObject *everyCallback2(ClarityObject *scope)
{
	assert(clarityBooleanGetValue(clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1"))) == TRUE);
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
		clarityFunctionObjectCreate(core, everyFunction1,
		clarityUndefined()));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, everyCallback1,
		clarityUndefined()));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "every"), parameters);

	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(core, everyFunction2,
		clarityUndefined()));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, everyCallback2,
		clarityUndefined()));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "every"), parameters);
}
