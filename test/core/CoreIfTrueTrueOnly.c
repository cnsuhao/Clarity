#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include <assert.h>

static Bool gotTrue = FALSE;

static ClarityObject *ifTrue(ClarityObject *scope)
{
	gotTrue = TRUE;
	return NULL;
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;
	ClarityObject *boolean;

	parameters = clarityObjectCreate(heap);
	boolean = clarityBooleanObjectCreate(heap, TRUE);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", boolean);
	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, ifTrue,
		NULL));
	clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "if"), parameters);
}
