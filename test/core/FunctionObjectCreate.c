#include "Clarity.h"
#include <assert.h>

static ClarityObject *testFunction(ClarityObject *context)
{
	return 0;
}

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;
	ClarityObject *context;

	context = clarityObjectCreate(heap);
	object = clarityFunctionObjectCreate(heap, testFunction, context);
	assert(object != 0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
