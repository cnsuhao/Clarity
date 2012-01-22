#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include "ClarityString.h"
#include "ClarityInteger.h"
#include <assert.h>

static ClarityString *dataString1;
static ClarityString *dataString2;
static ClarityString *dataString3;
static ClarityString *dataString4;

static void *testMap(ClarityString *string, Uint32 index, ClarityArray *array)
{
	void *retVal;

	retVal = NULL;
	assert(index < 3);

	if (string == dataString1)
		retVal = clarityIntegerCreate(clarityCore(), 1);
	else if (string == dataString2)
		retVal = clarityIntegerCreate(clarityCore(), 8);
	else if (string == dataString3)
		retVal = clarityIntegerCreate(clarityCore(), 6);
	else
		assert(FALSE);

	return retVal;
}

static void testMapDone(ClarityArray *array, ClarityString *string)
{
	ClarityInteger *integer;

	integer = clarityArrayPop(array);
	assert(clarityIntegerGetValue(integer) == 1);
	integer = clarityArrayPop(array);
	assert(clarityIntegerGetValue(integer) == 8);
	integer = clarityArrayPop(array);
	assert(clarityIntegerGetValue(integer) == 6);
	integer = clarityArrayPop(array);
	assert(integer == NULL);
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

	dataString1 = clarityStringCreate(core, data1);
	dataString2 = clarityStringCreate(core, data2);
	dataString3 = clarityStringCreate(core, data3);
	dataString4 = clarityStringCreate(core, data4);
	array = clarityArrayCreate(core);
	clarityArrayUnshift(array, dataString1);
	clarityArrayUnshift(array, dataString2);
	clarityArrayUnshift(array, dataString3);
	clarityArrayMap(array, (ClarityArrayMapFunction)testMap,
		(ClarityArrayMapCallback)testMapDone, dataString4);
}
