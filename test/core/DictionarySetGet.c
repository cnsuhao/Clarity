#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityMacro.h"
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

static void entry(Clarity *clarity, void *data)
{
	ClarityDictionary *dictionary;
	ClarityString *dataString;
	ClarityString *keyString;
	ClarityString *dataString2;
	const char *cDataString = "TestString";
	const char *cKeyString = "key";

	UNUSED(data);
	dataString = clarityStringCreate(clarity, cDataString);
	keyString = clarityStringCreate(clarity, cKeyString);
	dictionary = clarityDictionaryCreate(
		clarity,
		(ClarityComparator)clarityStringCompare);

	clarityDictionarySetObject(dictionary, keyString, dataString);
	dataString2 = clarityDictionaryGetObject(dictionary, keyString);
	assert(dataString2 != NULL);
	assert(clarityStringCompare(dataString, dataString2) == 0);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate(entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
