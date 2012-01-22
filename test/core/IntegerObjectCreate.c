#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityIntegerObject.h"
#include "ClarityInteger.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TEST_VALUE 23

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *object;
	ClarityInteger *integer;

	object = clarityIntegerObjectCreate(core, TEST_VALUE);
	integer = clarityObjectGetInnerData(object);
	assert(clarityIntegerGetValue(integer) == TEST_VALUE);
}
