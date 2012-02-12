#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityCore.h"
#include <assert.h>

#define TEST_VALUE "test string"

static ClarityObject *testFunction(ClarityObject *scope)
{
	ClarityObject *object = clarityObjectCreate(clarityHeap(scope));
	return object;
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)testFunction);
}


void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);

	clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "require"),
		clarityObjectSetMember(clarityObjectCreate(heap), "$1",
		clarityStringObjectCreate(heap, CLARITY_FILE)));
}
