#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static ClarityObject *testFunction(ClarityObject *context)
{
	return clarityUndefined();
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *object;
	ClarityObject *scope;

	scope = clarityObjectCreate(core);
	object = clarityFunctionObjectCreate(core, testFunction, scope);
	assert(clarityFunctionObjectCall(NULL, scope) == clarityUndefined());
	assert(clarityFunctionObjectCall(object, NULL) == clarityUndefined());
}

