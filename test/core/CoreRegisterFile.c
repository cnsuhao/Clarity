#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityStringObject.h"
#include "ClarityArrayObject.h"
#include "ClarityNumberObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityCore.h"
#include <assert.h>

#define TEST_VALUE "test string"

static ClarityObject *testFunction(ClarityObject *scope)
{
	return scope;
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		"entry", (ClarityFileInit)testFunction);
	clarityRegisterFile(0, 0, 0);
}

