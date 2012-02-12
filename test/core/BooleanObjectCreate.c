#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityBooleanObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *boolean;
	ClarityObject *integer;

	boolean = clarityBooleanObjectCreate(heap, TRUE);
	integer = clarityIntegerObjectCreate(heap, 23);
	assert(boolean != NULL);
	assert(clarityBooleanObjectGetValue(boolean) == TRUE);
	boolean = clarityBooleanObjectCreate(heap, FALSE);
	assert(boolean != NULL);
	assert(clarityBooleanObjectGetValue(boolean) == FALSE);
	assert(clarityBooleanObjectGetValue(NULL) == FALSE);
	assert(clarityBooleanObjectGetValue(integer) == FALSE);
}

