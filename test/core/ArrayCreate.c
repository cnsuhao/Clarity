#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityArray *array;

	array = clarityArrayCreate(heap);
	assert(array != NULL);
}
