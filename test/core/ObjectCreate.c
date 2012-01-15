#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityObject.h"
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
	ClarityObject *object;
	ClarityObject *subObject;

	object = clarityObjectCreate(core);
	assert(object != NULL);
	assert(clarityObjectGetInnerData(NULL) == NULL);
	clarityObjectLock(NULL);
	assert(clarityStrCmp(core,
		clarityObjectTypeOf(NULL), "undefined") == NULL);
	subObject = clarityObjectGetMember(object, NULL);
	assert(clarityUndefined() == subObject);
	subObject = clarityObjectGetMember(NULL, "string");
	assert(clarityUndefined() == subObject);
	subObject = clarityObjectCreate(core);
	assert(clarityObjectSetMember(
		NULL, "string", subObject) == clarityUndefined());
	assert(clarityObjectSetMember(
		object, NULL, subObject) == clarityUndefined());
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
