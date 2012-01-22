#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include "ClarityString.h"
#include "ClarityInteger.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static ClarityString *dataString1;
static ClarityString *dataString2;
static ClarityString *dataString3;
static ClarityString *dataString4;

static Bool testFilter(ClarityString *string, Uint32 index,
	ClarityArray *array)
{
	Bool retVal;

	retVal = FALSE;
	assert(index < 3);

	if (string == dataString1)
		retVal = TRUE;
	else if (string == dataString2)
		retVal = FALSE;
	else if (string == dataString3)
		retVal = TRUE;
	else
		assert(FALSE);

	return retVal;
}

static void testFilterDone(ClarityArray *array, ClarityString *doneString)
{
	ClarityString *string;

	string = clarityArrayPop(array);
	assert(clarityStringCompare(string, dataString1) == 0);
	string = clarityArrayPop(array);
	assert(clarityStringCompare(string, dataString3) == 0);
	string = clarityArrayPop(array);
	assert(string == NULL);
	assert(doneString == dataString4);
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
	clarityArrayFilter(array, (ClarityArrayTestFunction)testFilter,
		(ClarityArrayMapCallback)testFilterDone, dataString4);
}
