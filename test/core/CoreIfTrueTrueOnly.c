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

static ClarityObject *ifTrue(ClarityObject *scope)
{
	gotTrue = TRUE;
	return clarityUndefined();
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	ClarityObject *parameters;
	ClarityObject *boolean;

	parameters = clarityObjectCreate(core);
	boolean = clarityBooleanObjectCreate(core, TRUE);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", boolean);
	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, ifTrue,
		clarityUndefined()));
	clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "if"), parameters);
}
