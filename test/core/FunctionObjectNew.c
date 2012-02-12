#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

static ClarityObject *testFunction(ClarityObject *scope)
{
	ClarityObject *this = clarityObjectGetMember(scope, "this");

	clarityObjectSetMember(this, "testMember",
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
		clarityObjectSetMember(clarityObjectCreate(heap), "$1",
		clarityNumberObjectCreate(heap, 34)));
	assert(clarityNumberObjectGetValue(
		clarityObjectGetMember(newObject, "testMember")) == 34);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
