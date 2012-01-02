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

static ClarityString *dataString4;

static Bool testEveryWithFalse(ClarityString *string,
							   Uint32 index,
							   ClarityArray *array)
{
	assert(index < 3);
	return (index == 2);
}

static Bool testEveryAllTrue(ClarityString *string,
							 Uint32 index,
							 ClarityArray *array)
{
	assert(index < 3);
	return TRUE;
}

static void testEveryWithFalseDone(Bool every,
								   ClarityString *string)
{
	assert(every == FALSE);
	assert(string == dataString4);
}

static void testEveryAllTrueDone(Bool every,
								 ClarityString *string)
{
	assert(every == TRUE);
	assert(string == dataString4);
}

static void entry(ClarityCore *clarity)
{
	const char *data1 = "TestString1";
	const char *data2 = "TestString2";
	const char *data3 = "TestString3";
	const char *data4 = "TestString4";
	ClarityArray *array;
	ClarityString *dataString1;
	ClarityString *dataString2;
	ClarityString *dataString3;

	dataString1 = clarityStringCreate(clarity, data1);
	dataString2 = clarityStringCreate(clarity, data2);
	dataString3 = clarityStringCreate(clarity, data3);
	dataString4 = clarityStringCreate(clarity, data4);
	array = clarityArrayCreate(clarity);
	clarityArrayUnshift(array, dataString1);
	clarityArrayUnshift(array, dataString2);
	clarityArrayUnshift(array, dataString3);
	clarityArrayEvery(array,
					 (ClarityArrayTestFunction)testEveryAllTrue,
					 (ClarityArrayTestCallback)testEveryAllTrueDone,
					 dataString4);

	clarityArrayEvery(array,
					 (ClarityArrayTestFunction)testEveryWithFalse,
					 (ClarityArrayTestCallback)testEveryWithFalseDone,
					 dataString4);

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
