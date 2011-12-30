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

static void *testMap(ClarityString *string,
					Uint32 index,
					ClarityArray *array,
					Clarity *clarity)
{
	void *retVal;

	retVal = NULL;
	assert(index < 3);

	if (string == dataString1)
		retVal = clarityIntegerCreate(clarity, 1);
	else if (string == dataString2)
		retVal = clarityIntegerCreate(clarity, 8);
	else if (string == dataString3)
		retVal = clarityIntegerCreate(clarity, 6);
	else
		assert(FALSE);

	return retVal;
}

static void testMapDone(Clarity *clarity,
						ClarityArray *array,
						ClarityString *string)
{
	ClarityInteger *integer;

	integer = clarityArrayPop(array);
	assert(clarityIntegerGetUint32(integer) == 1);
	integer = clarityArrayPop(array);
	assert(clarityIntegerGetUint32(integer) == 8);
	integer = clarityArrayPop(array);
	assert(clarityIntegerGetUint32(integer) == 6);
	integer = clarityArrayPop(array);
	assert(integer == NULL);
	assert(string == dataString4);
}

static void entry(Clarity *clarity, void *data)
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
	clarityArrayMap(array,
					(ClarityArrayMapFunction)testMap,
					(ClarityArrayMapCallback)testMapDone,
					dataString4);
}

int main(void)
{
	ClarityHeap *heap;
	Clarity *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate(entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}