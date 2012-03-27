#include "Clarity.h"
#include <assert.h>

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;
	ClarityObject *subObject;

	object = clarityObjectCreate(heap);
	assert(object != 0);
	assert(clarityObjectGetInnerData(0) == 0);
	assert(clarityObjectIsTypeOf(0, "undefined"));
	subObject = clarityObjectGetMember(object, 0);
	assert(clarityObjectIsTypeOf(subObject, "undefined"));
	subObject = clarityObjectGetMember(0, "string");
	assert(clarityObjectIsTypeOf(subObject, "undefined"));
	subObject = clarityObjectCreate(heap);
	assert(clarityObjectIsTypeOf(clarityObjectSetOwnMember(
		0, "string", subObject), "undefined"));
	assert(clarityObjectIsTypeOf(clarityObjectSetOwnMember(
		object, 0, subObject), "object"));
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
