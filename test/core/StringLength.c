#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityString.h"
#include <assert.h>
#include <string.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	const char *testString = "TestString";
	ClarityString *string;

	string = clarityStringCreate(core, testString);
	assert(strlen(testString) == clarityStringLength(string));
}

