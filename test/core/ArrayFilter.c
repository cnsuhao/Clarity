#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include "ClarityString.h"
#include "ClarityInteger.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

static ClarityString *dataString1;
static ClarityString *dataString2;
static ClarityString *dataString3;
static ClarityString *dataString4;

static Bool testFilter(ClarityString *string,
					   Uint32 index,
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

static void testFilterDone(ClarityArray *array,
						   ClarityString *doneString)
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

static void entry(Clarity *clarity)
{
	const char *data1 = "TestString1";
	const char *data2 = "TestString2";
	const char *data3 = "TestString3";
	const char *data4 = "TestString4";
	ClarityArray *array;

	dataString1 = clarityStringCreate(clarity, data1);
	dataString2 = clarityStringCreate(clarity, data2);
	dataString3 = clarityStringCreate(clarity, data3);
	dataString4 = clarityStringCreate(clarity, data4);
	array = clarityArrayCreate(clarity);
	clarityArrayUnshift(array, dataString1);
	clarityArrayUnshift(array, dataString2);
	clarityArrayUnshift(array, dataString3);
	clarityArrayFilter(array,
					(ClarityArrayTestFunction)testFilter,
					(ClarityArrayMapCallback)testFilterDone,
					dataString4);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
