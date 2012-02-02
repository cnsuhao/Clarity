#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityIntegerObject.h"
#include <assert.h>

#define TEST_VALUE 23

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;

	object = clarityIntegerObjectCreate(heap, TEST_VALUE);
	assert(clarityIntegerObjectGetValue(object) == TEST_VALUE);
}
