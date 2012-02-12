#include "Clarity.h"
#include <assert.h>

static Bool gotTrue = 0;
static Bool gotFalse = 0;
static Bool gotDone = 0;

static ClarityObject *ifTrue(ClarityObject *scope)
{
	gotTrue = 1;
	return 0;
}

static ClarityObject *ifFalse(ClarityObject *scope)
{
	gotFalse = 1;
	return 0;
}

static ClarityObject *ifDone(ClarityObject *scope)
{
	gotDone = 1;
	assert(gotTrue);
	return 0;
}

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;
	ClarityObject *boolean;

	parameters = clarityObjectCreate(heap);
	boolean = clarityBooleanObjectCreate(heap, 1);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1", boolean);
	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(heap, ifTrue,
		0));
	clarityObjectSetMember(parameters, "$3",
		clarityFunctionObjectCreate(heap, ifFalse,
		0));
	clarityObjectSetMember(parameters, "$4",
		clarityFunctionObjectCreate(heap, ifDone,
		0));
	clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "if"), parameters);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)clarityEntry);
}
