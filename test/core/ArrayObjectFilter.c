#include "Clarity.h"
#include <assert.h>

static ClarityObject *arrayObject;
static Bool gotCallback = 0;

static ClarityObject *filterFunction(ClarityObject *scope)
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
	return clarityBooleanObjectCreate(clarityHeap(scope), index != 2);
}

static ClarityObject *filterCallback(ClarityObject *scope)
{
	ClarityArray *array = clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1"));

	assert(clarityNumberObjectGetValue(
		clarityArrayPop(array)) == 16);

	assert(clarityNumberObjectGetValue(
		clarityArrayPop(array)) == 4);

	assert(clarityNumberObjectGetValue(
		clarityArrayPop(array)) == 2);

	assert(clarityArrayPop(array) == 0);

	gotCallback = 1;
	return 0;
}

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;

	arrayObject = clarityArrayObjectCreate(heap);
	clarityArrayObjectPush(arrayObject, clarityNumberObjectCreate(heap, 2));
	clarityArrayObjectPush(arrayObject, clarityNumberObjectCreate(heap, 4));
	clarityArrayObjectPush(arrayObject, clarityNumberObjectCreate(heap, 8));
	clarityArrayObjectPush(arrayObject,
		clarityNumberObjectCreate(heap, 16));
	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", arrayObject);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(heap, filterFunction,
		0));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, filterCallback,
		0));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "filter"), parameters);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
