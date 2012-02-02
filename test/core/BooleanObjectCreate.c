#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityBooleanObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;

	object = clarityBooleanObjectCreate(heap, TRUE);
	assert(object != NULL);
	assert(clarityBooleanObjectGetValue(object) == TRUE);
	object = clarityBooleanObjectCreate(heap, FALSE);
	assert(object != NULL);
	assert(clarityBooleanObjectGetValue(object) == FALSE);
}

