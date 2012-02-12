#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityCore.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *string;

	string = clarityStringObjectCreate(heap, "test");
	assert(clarityObjectIsTypeOf(string, "string"));
	assert(!clarityObjectIsTypeOf(string, "integer"));
	assert(!clarityObjectIsTypeOf(string, NULL));
}

