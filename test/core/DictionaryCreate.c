#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityMacro.h"
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
	UNUSED(first);
	UNUSED(second);
	return 0;
}

static void entry(Clarity *clarity, void *data)
{
	ClarityDictionary *dictionary;

	UNUSED(data);
	dictionary = clarityDictionaryCreate(clarity, compare);
	assert(dictionary != NULL);
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
