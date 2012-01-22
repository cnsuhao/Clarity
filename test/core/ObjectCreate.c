#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
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
		object, NULL, subObject) == object);
}

