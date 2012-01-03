#include "ClarityHeap.h"
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

int main(void)
{
	ClarityHeap *heap;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	assert(heap != NULL);
	clarityHeapRetain(heap);
	assert(clarityHeapGetHeap(NULL) == NULL);
	clarityHeapRelease(heap);
	return 0;
}

