#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include <assert.h>

#define TEST_VALUE "test string"

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;

	object = clarityStringObjectCreate(heap, TEST_VALUE);
	assert(clarityStrCmp(clarityStringObjectGetValue(object),
		TEST_VALUE) == 0);
}

