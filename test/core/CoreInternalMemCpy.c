#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityMacro.h"
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

static void entry(Clarity *clarity, void *data)
{
	static const int BUFFER_SIZE = 11;
	const char *srcBuffer = "TestString";
	char dstBuffer[BUFFER_SIZE];

	UNUSED(data);
	clarityMemCpy(clarity, dstBuffer, srcBuffer, BUFFER_SIZE);
	assert(strcmp(srcBuffer, dstBuffer) == 0);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate(entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
