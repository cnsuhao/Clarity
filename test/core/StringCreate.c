#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityString.h"
#include <assert.h>
#include <string.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	const char *testString = "TestString";
	const char *cString;
	ClarityString *string;

	string = clarityStringCreate(core, testString);
	cString = clarityStringGetValue(string);

	assert(strcmp(testString, cString) == 0);
}

