#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityString.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_VALUE "test string"

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *object;
	ClarityString *string;

	object = clarityStringObjectCreate(core, TEST_VALUE);
	string = clarityObjectGetInnerData(object);
	assert(clarityStrCmp(core, clarityStringGetValue(string), TEST_VALUE) == 0);
}

