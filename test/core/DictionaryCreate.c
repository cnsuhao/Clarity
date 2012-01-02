#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityDictionary.h"
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

static Sint8 compare(void *first, void *second)
{
	return 0;
}

static void entry(ClarityCore *clarity)
{
	ClarityDictionary *dictionary;

	dictionary = clarityDictionaryCreate(clarity, compare);
	assert(dictionary != NULL);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
