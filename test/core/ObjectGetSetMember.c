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

static void entry(Clarity *clarity)
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
	ClarityString *baseDataString;
	ClarityString *baseDataString2;
	ClarityString *lessDataString;
	ClarityString *moreDataString;
	ClarityString *resultString;

	object = clarityObjectCreate(clarity);
	assert(object != NULL);

	baseDataString = clarityStringCreate(clarity, baseData);
	baseDataString2 = clarityStringCreate(clarity, baseData2);
	lessDataString = clarityStringCreate(clarity, lessData);
	moreDataString = clarityStringCreate(clarity, moreData);

	clarityObjectSetMember(object, baseKey, baseDataString);
	clarityObjectSetMember(object, baseKey, baseDataString2);
	clarityObjectSetMember(object, moreKey, moreDataString);
	clarityObjectSetMember(object, lessKey, lessDataString);
	resultString = clarityObjectGetMember(object, baseKey);
	assert(clarityStringCompare(baseDataString2, resultString) == 0);
	resultString = clarityObjectGetMember(object, lessKey);
	assert(clarityStringCompare(lessDataString, resultString) == 0);
	resultString = clarityObjectGetMember(object, moreKey);
	assert(clarityStringCompare(moreDataString, resultString) == 0);
	resultString = clarityObjectGetMember(object, missingKey);
	assert(resultString == NULL);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
