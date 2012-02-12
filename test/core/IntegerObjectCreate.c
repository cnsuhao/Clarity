#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityIntegerObject.h"
#include <assert.h>

#define TEST_VALUE 23

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *integer;
	ClarityObject *string;

	integer = clarityIntegerObjectCreate(heap, TEST_VALUE);
	string = clarityStringObjectCreate(heap, "test string");
	assert(clarityIntegerObjectGetValue(integer) == TEST_VALUE);
	assert(clarityIntegerObjectGetValue(NULL) == 0);
	assert(clarityIntegerObjectGetValue(string) == 0);
}
