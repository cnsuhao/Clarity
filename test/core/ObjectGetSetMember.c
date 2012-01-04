#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityObject.h"
#include "ClarityString.h"
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
	const char *baseData = "TestString1";
	const char *baseData2 = "TestString1.2";
	const char *lessData = "TestString2";
	const char *moreData = "TestString3";
	const char *baseKey = "Bkey";
	const char *lessKey = "Akey";
	const char *moreKey = "Ckey";
	const char *missingKey = "Dkey";
	ClarityObject *object;
	ClarityObject *baseDataString;
	ClarityObject *baseDataString2;
	ClarityObject *lessDataString;
	ClarityObject *moreDataString;
	ClarityObject *resultString;

	object = clarityObjectCreate(core);
	assert(object != NULL);

	baseDataString = clarityStringObjectCreate(core, baseData);
	baseDataString2 = clarityStringObjectCreate(core, baseData2);
	lessDataString = clarityStringObjectCreate(core, lessData);
	moreDataString = clarityStringObjectCreate(core, moreData);

	clarityObjectSetMember(object, baseKey, baseDataString);
	clarityObjectSetMember(object, baseKey, baseDataString2);
	clarityObjectSetMember(object, moreKey, moreDataString);
	clarityObjectSetMember(object, lessKey, lessDataString);
	resultString = clarityObjectGetMember(object, baseKey);
	assert(clarityStringCompare(
		(ClarityString *)clarityObjectGetInnerData(baseDataString2),
		(ClarityString *)clarityObjectGetInnerData(resultString)) == 0);
	resultString = clarityObjectGetMember(object, lessKey);
	assert(clarityStringCompare(
		(ClarityString *)clarityObjectGetInnerData(lessDataString),
		(ClarityString *)clarityObjectGetInnerData(resultString)) == 0);
	resultString = clarityObjectGetMember(object, moreKey);
	assert(clarityStringCompare(
		(ClarityString *)clarityObjectGetInnerData(moreDataString),
		(ClarityString *)clarityObjectGetInnerData(resultString)) == 0);
	resultString = clarityObjectGetMember(object, missingKey);
	assert(resultString == clarityObjectUndefined(core));
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
