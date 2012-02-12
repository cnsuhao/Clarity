#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include <assert.h>

#define TEST_VALUE "test string"

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *string;
	ClarityObject *integer;

	string = clarityStringObjectCreate(heap, TEST_VALUE);
	integer = clarityIntegerObjectCreate(heap, 2);
	assert(clarityStrCmp(clarityStringObjectGetValue(string),
		TEST_VALUE) == 0);
	assert(clarityStrCmp(clarityStringObjectGetValue(NULL), "") == 0);
	assert(clarityStrCmp(clarityStringObjectGetValue(integer), "") == 0);
}

