#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityNumberObject.h"
#include <assert.h>

#define TEST_VALUE 23

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *integer;
	ClarityObject *string;

	integer = clarityNumberObjectCreate(heap, TEST_VALUE);
	string = clarityStringObjectCreate(heap, "test string");
	assert(clarityNumberObjectGetValue(integer) == TEST_VALUE);
	assert(clarityNumberObjectGetValue(0) == 0);
	assert(clarityNumberObjectGetValue(string) == 0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}

