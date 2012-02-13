#include "Clarity.h"
#include <assert.h>

static ClarityObject *arrayObject;
static Bool gotCallback = 0;

static ClarityObject *everyFunction1(ClarityObject *scope)
{
	Uint32 data;
	Uint32 index;
	ClarityObject *array;

	data = clarityNumberObjectGetValue(
		clarityObjectGetOwnMember(scope, "$1"));

	index = clarityNumberObjectGetValue(
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
		clarityObjectGetOwnMember(scope, "$1")) == 0);
	gotCallback = 1;
	return 0;
}

static ClarityObject *everyFunction2(ClarityObject *scope)
{
	Uint32 data;
	Uint32 index;
	ClarityObject *array;

	data = clarityNumberObjectGetValue(
		clarityObjectGetOwnMember(scope, "$1"));

	index = clarityNumberObjectGetValue(
		clarityObjectGetOwnMember(scope, "$2"));

	assert((data == 2 && index == 0) ||
		(data == 4 && index == 1) ||
		(data == 8 && index == 2) ||
		(data == 16 && index == 3));

	array = clarityObjectGetOwnMember(scope, "$3");
	assert(array == arrayObject);
	return clarityBooleanObjectCreate(clarityHeap(scope), 1);
}

static ClarityObject *everyCallback2(ClarityObject *scope)
{
	assert(clarityBooleanObjectGetValue(
		clarityObjectGetOwnMember(scope, "$1")) == 1);
	gotCallback = 1;
	return 0;
}

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;

	arrayObject = clarityArrayObjectCreate(heap);
	clarityArrayObjectPush(arrayObject,
		clarityNumberObjectCreate(heap, 2));
	clarityArrayObjectPush(arrayObject,
		clarityNumberObjectCreate(heap, 4));
	clarityArrayObjectPush(arrayObject,
		clarityNumberObjectCreate(heap, 8));
	clarityArrayObjectPush(arrayObject,
		clarityNumberObjectCreate(heap, 16));
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(heap, everyFunction1,
		0));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, everyCallback1,
		0));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "every"), parameters);

	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(heap, everyFunction2,
		0));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, everyCallback2,
		0));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "every"), parameters);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
