#include "Clarity.h"
#include <assert.h>
#include <string.h>

#define TEST_VALUE "test string"

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *string;
	ClarityObject *integer;

	string = clarityStringObjectCreate(heap, TEST_VALUE);
	integer = clarityNumberObjectCreate(heap, 2);
	assert(strcmp(clarityStringObjectGetValue(string),
		TEST_VALUE) == 0);
	assert(strcmp(clarityStringObjectGetValue(0), "") == 0);
	assert(strcmp(clarityStringObjectGetValue(integer), "") == 0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
