#include "Clarity.h"
#include "ClarityHeap.h"
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

static void entry(ClarityCore *core)
{
	static const int BUFFER_SIZE = 11;
	const char *srcBuffer = "TestString";
	char dstBuffer[BUFFER_SIZE];

	clarityMemCpy(core, dstBuffer, srcBuffer, BUFFER_SIZE);
	assert(strcmp(srcBuffer, dstBuffer) == 0);
}

static void *testMemCpy(ClarityCore *core, void *dstData,
	const void *srcData, Uint32 size)
{
	return memcpy(dstData, srcData, size);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	claritySetMemCpy(core, testMemCpy);
	clarityStart(core);
	clarityStop(core);
	return 0;
}
