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

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

static ClarityObject *ifTrue(ClarityObject *scope)
{
	gotTrue = TRUE;
	return clarityUndefined();
}

static void entry(ClarityCore *core)
{
	ClarityObject *parameters;
	ClarityObject *boolean;
	ClarityObject *global = clarityGlobal(core);

	parameters = clarityObjectCreate(core);
	boolean = clarityBooleanObjectCreate(core, FALSE);
	clarityObjectSetMember(parameters, "this", global);
	clarityObjectSetMember(parameters, "$1", boolean);
	clarityObjectSetMember(parameters, "$2",
		clarityFunctionObjectCreate(core, ifTrue,
		clarityUndefined()));
	clarityFunctionObjectCall(
		clarityObjectGetMember(global, "if"), parameters);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(core);
	clarityStop(core);
	assert(!gotTrue);
	return 0;
}
