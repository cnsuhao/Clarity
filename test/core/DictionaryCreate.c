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

static void entry(ClarityCore *core)
{
	ClarityDictionary *dictionary;

	dictionary = clarityDictionaryCreate(core, compare);
	assert(dictionary != NULL);
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
