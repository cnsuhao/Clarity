#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include <assert.h>

static Bool gotTrue = FALSE;
static Bool gotFalse = FALSE;
static Bool gotDone = FALSE;
static Bool gotCondition = FALSE;

static ClarityObject *ifTrue(ClarityObject *scope)
{
	gotTrue = TRUE;
	return NULL;
}

static ClarityObject *ifFalse(ClarityObject *scope)
{
	gotFalse = TRUE;
	return NULL;
}

static ClarityObject *ifDone(ClarityObject *scope)
{
	gotDone = TRUE;
	assert(gotTrue);
	return NULL;
}

static ClarityObject *condition(ClarityObject *scope)
{
	gotCondition = TRUE;
	return clarityBooleanObjectCreate(clarityHeap(scope), TRUE);
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;

	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(heap, condition,
		NULL));
	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, ifTrue,
		NULL));
	clarityObjectSetMember(parameters, "$3",
		clarityFunctionObjectCreate(heap, ifFalse,
		NULL));
	clarityObjectSetMember(parameters, "$4",
		clarityFunctionObjectCreate(heap, ifDone,
		NULL));
	clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "if"), parameters);
}
