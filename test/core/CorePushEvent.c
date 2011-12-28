#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityString.h"
#include "ClarityMacro.h"
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

static void event1(Clarity *clarity, ClarityString *string)
{
	const char *cString;
	
	cString = clarityStringGetCString(string);
	assert(strcmp(cString, TEST_STRING) == 0);
	assert(gotEvent1 == FALSE);
	assert(gotEvent2 == TRUE);
	gotEvent1 = TRUE;
}

static void event2(Clarity *clarity, ClarityString *string)
{
	const char *cString;
	
	cString = clarityStringGetCString(string);
	assert(strcmp(cString, TEST_STRING) == 0);
	assert(gotEvent1 == FALSE);
	assert(gotEvent2 == FALSE);
	gotEvent2 = TRUE;
}


static void entry(Clarity *clarity, void *data)
{
	UNUSED(data);
	clarityEnqueueEvent(clarity,
						(ClarityEventFunction)event1,
						clarityStringCreate(clarity, TEST_STRING));

	clarityPushEvent(clarity,
					 (ClarityEventFunction)event2,
					 clarityStringCreate(clarity, TEST_STRING));

}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate(entry, heap);
	clarityStart(clarity);
	assert(gotEvent1 == TRUE);
	assert(gotEvent2 == TRUE);
	clarityStop(clarity);
	return 0;
}
