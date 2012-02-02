#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;
	ClarityArray *array;

	array = clarityArrayCreate(heap);
	object = clarityArrayObjectCreate(heap, array);
	assert(object != NULL);
}
