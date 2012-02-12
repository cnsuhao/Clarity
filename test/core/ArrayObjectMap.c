#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include "ClarityFunctionObject.h"
#include <assert.h>

static ClarityObject *arrayObject;
static Bool gotCallback = 0;

static ClarityObject *mapFunction(ClarityObject *scope)
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
	return clarityNumberObjectCreate(clarityHeap(scope), data * 2);
}

static ClarityObject *mapCallback(ClarityObject *scope)
{
	ClarityArray *array = clarityObjectGetInnerData(
		clarityObjectGetOwnMember(scope, "$1"));

	assert(clarityNumberObjectGetValue(
		clarityArrayPop(array)) == 32);

	assert(clarityNumberObjectGetValue(
		clarityArrayPop(array)) == 16);

	assert(clarityNumberObjectGetValue(
		clarityArrayPop(array)) == 8);

	assert(clarityNumberObjectGetValue(
		clarityArrayPop(array)) == 4);

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
		clarityFunctionObjectCreate(heap, mapFunction,
		0));

	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, mapCallback,
		0));

	clarityFunctionObjectCall(
		clarityObjectGetMember(arrayObject, "map"), parameters);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
