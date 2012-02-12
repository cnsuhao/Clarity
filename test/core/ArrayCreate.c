#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include <assert.h>

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityArray *array;

	array = clarityArrayCreate(heap);
	assert(array != 0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
