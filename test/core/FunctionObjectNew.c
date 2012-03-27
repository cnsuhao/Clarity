#include "Clarity.h"
#include <assert.h>

static ClarityObject *testFunction(ClarityObject *scope)
{
	ClarityObject *this = clarityObjectGetMember(scope, "this");

	clarityObjectSetOwnMember(this, "testMember",
		clarityObjectGetOwnMember(scope, "$1"));
	return 0;
}

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *function;
	ClarityObject *scope;
	ClarityObject *newObject;

	scope = clarityObjectCreate(heap);
	function = clarityFunctionObjectCreate(heap, testFunction, scope);
	newObject = clarityFunctionObjectNew(function,
		clarityObjectSetOwnMember(clarityObjectCreate(heap), "$1",
		clarityNumberObjectCreate(heap, 34)));
	assert(clarityNumberObjectGetValue(
		clarityObjectGetMember(newObject, "testMember")) == 34);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
