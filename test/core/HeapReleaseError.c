#include "Clarity.h"
#include "ClarityHeap.h"
#include <assert.h>

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	void *data = clarityHeapAllocate(heap, 200);
	char *errorPointer = (char *)data + 100;
	clarityMemSet(data, 0, 200);
	clarityHeapRelease(errorPointer);
	clarityHeap(errorPointer);
	clarityHeapRelease(data);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
