#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *object;
	ClarityObject *subObject;

	object = clarityObjectCreate(heap);
	assert(object != NULL);
	assert(clarityObjectGetInnerData(NULL) == NULL);
	clarityObjectLock(NULL);
	assert(clarityStrCmp(
		clarityObjectTypeOf(NULL), "undefined") == NULL);
	subObject = clarityObjectGetMember(object, NULL);
	assert(clarityStrCmp(clarityObjectTypeOf(subObject), "undefined") == 0);
	subObject = clarityObjectGetMember(NULL, "string");
	assert(clarityStrCmp(clarityObjectTypeOf(subObject), "undefined") == 0);
	subObject = clarityObjectCreate(heap);
	assert(clarityStrCmp(clarityObjectTypeOf(clarityObjectSetMember(
		NULL, "string", subObject)), "undefined") == 0);
	assert(clarityStrCmp(clarityObjectTypeOf(clarityObjectSetMember(
		object, NULL, subObject)), "object") == 0);
}

