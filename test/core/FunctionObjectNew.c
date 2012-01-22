#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArrayObject.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static ClarityObject *testFunction(ClarityObject *scope)
{
	ClarityObject *this = clarityObjectGetMember(scope, "this");

	clarityObjectSetMember(this, "testMember",
		clarityObjectGetOwnMember(scope, "$1"));
	return clarityUndefined();
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *function;
	ClarityObject *scope;
	ClarityObject *newObject;

	scope = clarityObjectCreate(core);
	function = clarityFunctionObjectCreate(core, testFunction, scope);
	newObject = clarityFunctionObjectNew(function,
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityIntegerObjectCreate(core, 34)));
	assert(clarityIntegerGetValue(
		clarityObjectGetInnerData(
		clarityObjectGetMember(newObject, "testMember"))) == 34);
}

