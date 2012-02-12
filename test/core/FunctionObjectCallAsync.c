#include "Clarity.h"
#include "ClarityCore.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

static ClarityObject *testFunction(ClarityObject *context)
{
	return 0;
}

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;
	ClarityObject *scope;
	ClarityObject *parameters;

	scope = clarityObjectCreate(heap);
	parameters = clarityObjectCreate(heap);
	object = clarityFunctionObjectCreateAsync(heap, testFunction, scope);
	clarityFunctionObjectCall(object, parameters);
	assert(clarityObjectIsTypeOf(
		clarityFunctionObjectCall(0, parameters), "undefined"));
	assert(clarityObjectIsTypeOf(
		clarityFunctionObjectCall(object, 0), "undefined"));
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
