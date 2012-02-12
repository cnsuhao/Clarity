#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	const char *baseData = "TestString1";
	const char *baseData2 = "TestString1.2";
	const char *lessData = "TestString2";
	const char *moreData = "TestString3";
	const char *baseKey = "Bkey";
	const char *lessKey = "Akey";
	const char *moreKey = "Ckey";
	const char *missingKey = "Dkey";
	ClarityObject *object;
	ClarityObject *object2;
	ClarityObject *baseDataString;
	ClarityObject *baseDataString2;
	ClarityObject *lessDataString;
	ClarityObject *moreDataString;
	ClarityObject *resultString;

	object = clarityObjectCreate(heap);

	baseDataString = clarityStringObjectCreate(heap, baseData);
	baseDataString2 = clarityStringObjectCreate(heap, baseData2);
	lessDataString = clarityStringObjectCreate(heap, lessData);
	moreDataString = clarityStringObjectCreate(heap, moreData);

	clarityObjectSetMember(object, baseKey, baseDataString);
	clarityObjectSetMember(object, baseKey, baseDataString);
	clarityObjectSetMember(object, baseKey, baseDataString2);
	clarityObjectSetMember(object, moreKey, moreDataString);
	clarityObjectSetMember(object, lessKey, lessDataString);
	resultString = clarityObjectGetMember(object, baseKey);
	assert(clarityStrCmp(
		clarityStringObjectGetValue(baseDataString2),
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityObjectGetMember(object, lessKey);
	assert(clarityStrCmp(clarityStringObjectGetValue(lessDataString),
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityObjectGetMember(object, moreKey);
	assert(clarityStrCmp(
		clarityStringObjectGetValue(moreDataString),
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityObjectGetMember(object, missingKey);
	assert(clarityStrCmp(clarityObjectTypeOf(resultString),
		"undefined") == 0);
	object2 = clarityObjectCreate(heap);
	clarityObjectSetMember(object2, missingKey, baseDataString2);
	clarityObjectSetMember(object, "prototype", object2);
	resultString = clarityObjectGetMember(object, missingKey);
	assert(clarityStrCmp(
		clarityStringObjectGetValue(baseDataString2),
		clarityStringObjectGetValue(resultString)) == 0);
}

