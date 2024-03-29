#include "Clarity.h"
#include <assert.h>

#define TEST_VALUE1 "test string"
#define TEST_VALUE2 "other test string"

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	Bool equals;
	ClarityObject *parameters;
	ClarityObject *object1;
	ClarityObject *object2;
	ClarityObject *object3;
	ClarityObject *object4;

	object1 = clarityStringObjectCreate(heap, TEST_VALUE1);
	object2 = clarityStringObjectCreate(heap, TEST_VALUE1);
	object3 = clarityStringObjectCreate(heap, TEST_VALUE2);
	object4 = clarityNumberObjectCreate(heap, 23);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetOwnMember(parameters, "this", object1);
	clarityObjectSetOwnMember(parameters, "$1", object2);
	equals = clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters));
	assert(equals);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetOwnMember(parameters, "this", object1);
	clarityObjectSetOwnMember(parameters, "$1", object3);
	equals = clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters));
	assert(!equals);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetOwnMember(parameters, "this", object1);
	clarityObjectSetOwnMember(parameters, "$1", object4);
	equals = clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object1, "equals"), parameters));
	assert(!equals);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
