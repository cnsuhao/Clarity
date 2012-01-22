#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *object;
	ClarityArray *array;

	array = clarityArrayCreate(core);
	object = clarityArrayObjectCreate(core, array);
	assert(object != NULL);
}
