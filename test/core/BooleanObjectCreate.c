#include "Clarity.h"
#include <assert.h>

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *boolean;
	ClarityObject *integer;

	boolean = clarityBooleanObjectCreate(heap, 1);
	integer = clarityNumberObjectCreate(heap, 23);
	assert(boolean != 0);
	assert(clarityBooleanObjectGetValue(boolean) == 1);
	boolean = clarityBooleanObjectCreate(heap, 0);
	assert(boolean != 0);
	assert(clarityBooleanObjectGetValue(boolean) == 0);
	assert(clarityBooleanObjectGetValue(0) == 0);
	assert(clarityBooleanObjectGetValue(integer) == 0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
