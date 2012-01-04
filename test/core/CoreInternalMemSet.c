#include "Clarity.h"
#include "ClarityHeap.h"
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
	static const int BUFFER_SIZE = 3;
	char buffer[BUFFER_SIZE];

	buffer[0] = 'a';
	buffer[1] = 'b';
	buffer[2] = 'c';
	clarityMemSet(core, buffer, 'd', BUFFER_SIZE);
	assert(strncmp(buffer, "ddd", BUFFER_SIZE) == 0);
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
