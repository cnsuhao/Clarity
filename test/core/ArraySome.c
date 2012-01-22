#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include "ClarityString.h"
#include "ClarityInteger.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static ClarityString *dataString4;

static Bool testSomeWithTrue(ClarityString *string, Uint32 index,
	ClarityArray *array)
{
	assert(index < 3);
	return (index != 2);
}

static Bool testSomeAllFalse(ClarityString *string, Uint32 index,
	ClarityArray *array)
{
	assert(index < 3);
	return FALSE;
}

static void testSomeWithTrueDone(Bool some, ClarityString *string)
{
	assert(some == TRUE);
	assert(string == dataString4);
}

static void testSomeAllFalseDone(Bool some, ClarityString *string)
{
	assert(some == FALSE);
	assert(string == dataString4);
}

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	const char *data1 = "TestString1";
	const char *data2 = "TestString2";
	const char *data3 = "TestString3";
	const char *data4 = "TestString4";
	ClarityArray *array;
	ClarityString *dataString1;
	ClarityString *dataString2;
	ClarityString *dataString3;

	dataString1 = clarityStringCreate(core, data1);
	dataString2 = clarityStringCreate(core, data2);
	dataString3 = clarityStringCreate(core, data3);
	dataString4 = clarityStringCreate(core, data4);
	array = clarityArrayCreate(core);
	clarityArrayUnshift(array, dataString1);
	clarityArrayUnshift(array, dataString2);
	clarityArrayUnshift(array, dataString3);
	clarityArraySome(array, (ClarityArrayTestFunction)testSomeAllFalse,
		(ClarityArrayTestCallback)testSomeAllFalseDone, dataString4);

	clarityArraySome(array, (ClarityArrayTestFunction)testSomeWithTrue,
		(ClarityArrayTestCallback)testSomeWithTrueDone, dataString4);

}

