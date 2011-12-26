#include "Clarity.h"
#include "ClarityHeap.h"
#include <stdlib.h>

static void entry(Clarity *clarity, void *data)
{

}

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

void clarity(void)
{
	Clarity *clarity;
	ClarityHeap *heap;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate(entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
}
