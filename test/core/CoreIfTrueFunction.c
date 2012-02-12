#include "Clarity.h"
#include <assert.h>

static Bool gotTrue = 0;
static Bool gotFalse = 0;
static Bool gotDone = 0;
static Bool gotCondition = 0;

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

static ClarityObject *condition(ClarityObject *scope)
{
	gotCondition = 1;
	return clarityBooleanObjectCreate(clarityHeap(scope), 1);
}

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	ClarityObject *parameters;

	parameters = clarityObjectCreate(heap);
	clarityObjectSetMember(parameters, "this", globalScope);
	clarityObjectSetMember(parameters, "$1",
		clarityFunctionObjectCreate(heap, condition,
		0));
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
