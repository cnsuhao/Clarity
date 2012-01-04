#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityString.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static Bool gotEvent1 = FALSE;
static Bool gotEvent2 = FALSE;
static const char *TEST_STRING = "TestString";

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

static void event1(ClarityString *string)
{
	const char *cString;

	cString = clarityStringGetCString(string);
	assert(strcmp(cString, TEST_STRING) == 0);
	assert(gotEvent1 == FALSE);
	assert(gotEvent2 == FALSE);
	gotEvent1 = TRUE;
}

static void event2(ClarityString *string)
{
	const char *cString;

	cString = clarityStringGetCString(string);
	assert(strcmp(cString, TEST_STRING) == 0);
	assert(gotEvent1 == TRUE);
	assert(gotEvent2 == FALSE);
	gotEvent2 = TRUE;
}

static void entry(ClarityCore *core)
{
	clarityEnqueueEvent(core, (ClarityEvent)event1,
		clarityStringCreate(core, TEST_STRING));

	clarityEnqueueEvent(core, (ClarityEvent)event2,
		clarityStringCreate(core, TEST_STRING));

}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(core);
	assert(gotEvent1 == TRUE);
	assert(gotEvent2 == TRUE);
	clarityStop(core);
	return 0;
}
