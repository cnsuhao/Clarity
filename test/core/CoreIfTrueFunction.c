#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static Bool gotTrue = FALSE;
static Bool gotFalse = FALSE;
static Bool gotDone = FALSE;
static Bool gotCondition = FALSE;

static ClarityObject *ifTrue(ClarityObject *scope)
{
	gotTrue = TRUE;
	return clarityUndefined();
}

static ClarityObject *ifFalse(ClarityObject *scope)
{
	gotFalse = TRUE;
	return clarityUndefined();
}

static ClarityObject *ifDone(ClarityObject *scope)
{
	gotDone = TRUE;
	assert(gotTrue);
	return clarityUndefined();
}

static ClarityObject *condition(ClarityObject *scope)
{
	gotCondition = TRUE;
	return clarityBooleanObjectCreate(clarityCore(), TRUE);
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *parameters;

	parameters = clarityObjectCreate(core);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(core, condition,
		clarityUndefined()));
	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, ifTrue,
		clarityUndefined()));
	clarityObjectSetMember(parameters, "$3",
		clarityFunctionObjectCreate(core, ifFalse,
		clarityUndefined()));
	clarityObjectSetMember(parameters, "$4",
		clarityFunctionObjectCreate(core, ifDone,
		clarityUndefined()));
	clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "if"), parameters);
}
