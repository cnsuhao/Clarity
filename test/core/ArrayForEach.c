#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include "ClarityString.h"
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

static void testForEach(ClarityString *string,
						Uint32 index,
						ClarityArray *array)
{
	assert(index < 3);
	assert(string == dataString1 ||
		   string == dataString2 ||
		   string == dataString3);
}

static void testForEachDone(ClarityString *string)
{
	assert(string == dataString4);
}

static void entry(ClarityCore *clarity)
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
	clarityArrayForEach(array,
						(ClarityArrayForEachFunction)testForEach,
						(ClarityArrayForEachCallback)testForEachDone,
						dataString4);
	clarityArrayForEachWithoutCallback(array,
						(ClarityArrayForEachFunction)testForEach);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *clarity;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	clarity = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(clarity);
	clarityStop(clarity);
	return 0;
}
