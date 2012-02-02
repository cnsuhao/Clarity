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
	assert(gotFalse);
	return NULL;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;
	ClarityObject *boolean;

	parameters = clarityObjectCreate(heap);
	boolean = clarityBooleanObjectCreate(heap, FALSE);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", boolean);
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
