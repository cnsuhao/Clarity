#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityArray *array;

	array = clarityArrayCreate(core);
	assert(array != NULL);
}
