#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityMacro.h"
#include <stdlib.h>
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
	UNUSED(data);
	UNUSED(clarity);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate(entry, heap);
	clarityStart(clarity);
	assert(clarityGetHeap(clarity) == heap);
	clarityStop(clarity);
	return 0;
}
