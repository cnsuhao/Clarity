#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityObject.h"
#include <assert.h>

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;
	ClarityObject *subObject;

	object = clarityObjectCreate(heap);
	assert(object != 0);
	assert(clarityObjectGetInnerData(0) == 0);
	clarityObjectLock(0);
	assert(clarityStrCmp(
		clarityObjectTypeOf(0), "undefined") == 0);
	subObject = clarityObjectGetMember(object, 0);
	assert(clarityStrCmp(clarityObjectTypeOf(subObject), "undefined") == 0);
	subObject = clarityObjectGetMember(0, "string");
	assert(clarityStrCmp(clarityObjectTypeOf(subObject), "undefined") == 0);
	subObject = clarityObjectCreate(heap);
	assert(clarityStrCmp(clarityObjectTypeOf(clarityObjectSetMember(
		0, "string", subObject)), "undefined") == 0);
	assert(clarityStrCmp(clarityObjectTypeOf(clarityObjectSetMember(
		object, 0, subObject)), "object") == 0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
