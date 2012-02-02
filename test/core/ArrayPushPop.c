#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include "ClarityStringObject.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	const char *data1 = "TestString1";
	const char *data2 = "TestString2";
	const char *data3 = "TestString3";
	ClarityArray *array;
	Uint32 length;
	ClarityObject *dataString1;
	ClarityObject *dataString2;
	ClarityObject *dataString3;
	ClarityObject *resultString;

	dataString1 = clarityStringObjectCreate(heap, data1);
	dataString2 = clarityStringObjectCreate(heap, data2);
	dataString3 = clarityStringObjectCreate(heap, data3);
	array = clarityArrayCreate(heap);
	clarityArrayPush(array, dataString1);
	clarityArrayPush(array, dataString2);
	clarityArrayPush(array, dataString3);
	resultString = clarityArrayPop(array);
	assert(clarityStrCmp(data3,
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityArrayPop(array);
	assert(clarityStrCmp(data2,
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityArrayPop(array);
	assert(clarityStrCmp(data1,
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityArrayPop(array);
	assert(resultString == NULL);
	clarityArrayPush(array, dataString1);
	clarityArrayPush(array, dataString2);
	clarityArrayPush(array, dataString3);
	clarityArrayLock(array);
	length = clarityArrayLength(array);
	assert(clarityArrayPop(array) == NULL);
	assert(length == clarityArrayLength(array));
	clarityArrayPush(array, dataString3);
	assert(length == clarityArrayLength(array));
	clarityArrayPush(NULL, dataString3);
}

