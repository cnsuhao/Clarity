#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
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
	Uint32 length;
	ClarityObject *parameters;
	ClarityObject *object;

	object = clarityStringObjectCreate(core, TEST_VALUE);
	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", object);
	length = clarityIntegerGetValue((ClarityInteger *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object, "length"), parameters)));
	assert(clarityStrLen(core, TEST_VALUE) == length);
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
