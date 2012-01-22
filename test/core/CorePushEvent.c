#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityString.h"
#include <assert.h>
#include <string.h>

static Bool gotEvent1 = FALSE;
static Bool gotEvent2 = FALSE;
static const char *TEST_STRING = "TestString";

static void event1(ClarityString *string)
{
	const char *cString;

	cString = clarityStringGetValue(string);
	assert(strcmp(cString, TEST_STRING) == 0);
	assert(gotEvent1 == FALSE);
	assert(gotEvent2 == TRUE);
	gotEvent1 = TRUE;
}

static void event2(ClarityString *string)
{
	const char *cString;

	cString = clarityStringGetValue(string);
	assert(strcmp(cString, TEST_STRING) == 0);
	assert(gotEvent1 == FALSE);
	assert(gotEvent2 == FALSE);
	gotEvent2 = TRUE;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	clarityEnqueueEvent(core, (ClarityEvent)event1,
		clarityStringCreate(core, TEST_STRING));

	clarityPushEvent(core, (ClarityEvent)event2,
		clarityStringCreate(core, TEST_STRING));

}
