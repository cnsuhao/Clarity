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

static void entry(Clarity *clarity, void *data)
{
	const char *data1 = "TestString1";
	const char *data2 = "TestString2";
	const char *data3 = "TestString3";
	ClarityArray *array;
	ClarityString *dataString1;
	ClarityString *dataString2;
	ClarityString *dataString3;
	ClarityString *resultString;

	dataString1 = clarityStringCreate(clarity, data1);
	dataString2 = clarityStringCreate(clarity, data2);
	dataString3 = clarityStringCreate(clarity, data3);
	array = clarityArrayCreate(clarity);
	clarityArrayPush(array, dataString1);
	clarityArrayPush(array, dataString2);
	clarityArrayPush(array, dataString3);
	resultString = clarityArrayPop(array);
	assert(clarityStringCompare(dataString3, resultString) == 0);
	resultString = clarityArrayPop(array);
	assert(clarityStringCompare(dataString2, resultString) == 0);
	resultString = clarityArrayPop(array);
	assert(clarityStringCompare(dataString1, resultString) == 0);
	resultString = clarityArrayPop(array);
	assert(resultString == NULL);
	clarityArrayPush(array, dataString1);
	clarityArrayPush(array, dataString2);
	clarityArrayPush(array, dataString3);
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
