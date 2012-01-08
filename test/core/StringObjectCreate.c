#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityString.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

#define TEST_VALUE "test string"

static void entry(ClarityCore *core)
{
	ClarityObject *object;
	ClarityString *string;

	object = clarityStringObjectCreate(core, TEST_VALUE);
	string = clarityObjectGetInnerData(object);
	assert(clarityStrCmp(core, clarityStringGetValue(string), TEST_VALUE) == 0);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(core);
	clarityStop(core);
	return 0;
}
