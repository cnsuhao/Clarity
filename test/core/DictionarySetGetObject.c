#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityDictionary.h"
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
	ClarityDictionary *dictionary;
	ClarityString *baseDataString;
	ClarityString *baseDataString2;
	ClarityString *lessDataString;
	ClarityString *moreDataString;
	ClarityString *baseKeyString;
	ClarityString *lessKeyString;
	ClarityString *moreKeyString;
	ClarityString *resultString;
	ClarityString *missingKeyString;

	dictionary = clarityDictionaryCreate(
		clarity,
		(ClarityComparator)clarityStringCompare);

	assert(dictionary != NULL);
	baseDataString = clarityStringCreate(clarity, baseData);
	baseDataString2 = clarityStringCreate(clarity, baseData2);
	lessDataString = clarityStringCreate(clarity, lessData);
	moreDataString = clarityStringCreate(clarity, moreData);
	baseKeyString = clarityStringCreate(clarity, baseKey);
	lessKeyString = clarityStringCreate(clarity, lessKey);
	moreKeyString = clarityStringCreate(clarity, moreKey);
	missingKeyString = clarityStringCreate(clarity, missingKey);

	clarityDictionarySetObject(dictionary, baseKeyString, baseDataString);
	clarityDictionarySetObject(dictionary, baseKeyString, baseDataString2);
	clarityDictionarySetObject(dictionary, moreKeyString, moreDataString);
	clarityDictionarySetObject(dictionary, lessKeyString, lessDataString);
	resultString = clarityDictionaryGetObject(dictionary, baseKeyString);
	assert(clarityStringCompare(baseDataString2, resultString) == 0);
	resultString = clarityDictionaryGetObject(dictionary, lessKeyString);
	assert(clarityStringCompare(lessDataString, resultString) == 0);
	resultString = clarityDictionaryGetObject(dictionary, moreKeyString);
	assert(clarityStringCompare(moreDataString, resultString) == 0);
	resultString = clarityDictionaryGetObject(dictionary, missingKeyString);
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
