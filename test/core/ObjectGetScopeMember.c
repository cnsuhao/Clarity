#include "Clarity.h"
#include <assert.h>
#include <string.h>

static ClarityObject *clarityEntry(ClarityObject *globalScope)
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

	clarityObjectSetOwnMember(object, baseKey, baseDataString);
	clarityObjectSetOwnMember(object, baseKey, baseDataString);
	clarityObjectSetOwnMember(object, baseKey, baseDataString2);
	clarityObjectSetOwnMember(object, moreKey, moreDataString);
	clarityObjectSetOwnMember(object, lessKey, lessDataString);

	object2 = clarityObjectCreate(heap);

	clarityObjectSetOwnMember(object2, missingKey, baseDataString2);

	clarityObjectSetOwnMember(object, "prototype", object2);

	resultString = clarityObjectGetMember(object, missingKey);
	assert(strcmp(
		clarityStringObjectGetValue(baseDataString2),
		clarityStringObjectGetValue(resultString)) == 0);
	clarityObjectSetMember(object, missingKey, baseDataString);
	resultString = clarityObjectGetOwnMember(object, missingKey);
	assert(clarityObjectIsTypeOf(resultString, "undefined"));
	resultString = clarityObjectGetMember(object, missingKey);
	assert(strcmp(
		clarityStringObjectGetValue(baseDataString),
		clarityStringObjectGetValue(resultString)) == 0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
