#include "ClarityHeap.h"
#include <stdlib.h>

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

int main(void)
{
	ClarityHeap *heap;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarityHeapRelease(heap, heap);
	return 0;
}
