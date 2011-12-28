#include "ClarityHeap.h"
#include <stdlib.h>
#include <assert.h>

int main(void)
{
	ClarityHeap *heap;

	heap = clarityHeapCreate(0, 1024, 32);
	clarityHeapRelease(heap, heap);
	return 0;
}
