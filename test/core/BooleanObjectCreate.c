#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *object;
	ClarityBoolean *boolean;

	object = clarityBooleanObjectCreate(core, TRUE);
	assert(object != NULL);
	boolean = clarityObjectGetInnerData(object);
	assert(clarityBooleanGetValue(boolean) == TRUE);
	object = clarityBooleanObjectCreate(core, FALSE);
	assert(object != NULL);
	boolean = clarityObjectGetInnerData(object);
	assert(clarityBooleanGetValue(boolean) == FALSE);
}

