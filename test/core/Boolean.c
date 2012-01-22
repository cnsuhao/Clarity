#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityBoolean *boolean;

	boolean = clarityBooleanCreate(core, TRUE);
	assert(clarityBooleanGetValue(boolean) == TRUE);
	assert(clarityBooleanGetValue(NULL) == FALSE);
}

