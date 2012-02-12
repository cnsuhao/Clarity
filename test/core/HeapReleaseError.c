#include "Clarity.h"
#include "ClarityHeap.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	void *data = clarityHeapAllocate(clarityHeap(globalScope), 200);
	char *errorPointer = (char *)data + 100;
	clarityMemSet(data, 0, 200);
	clarityHeapRelease(errorPointer);
	clarityHeap(errorPointer);
	clarityHeapRelease(data);
}

