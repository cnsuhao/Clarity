#include "Clarity.h"
#include "ClarityHeap.h"
#include "ClarityArray.h"
#include "ClarityString.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();
	const char *data1 = "TestString1";
	const char *data2 = "TestString2";
	const char *data3 = "TestString3";
	ClarityArray *array;
	Uint32 length;
	ClarityString *dataString1;
	ClarityString *dataString2;
	ClarityString *dataString3;
	ClarityString *resultString;

	dataString1 = clarityStringCreate(core, data1);
	dataString2 = clarityStringCreate(core, data2);
	dataString3 = clarityStringCreate(core, data3);
	array = clarityArrayCreate(core);
	clarityArrayUnshift(array, dataString1);
	clarityArrayUnshift(array, dataString2);
	clarityArrayUnshift(array, dataString3);
	resultString = clarityArrayShift(array);
	assert(clarityStringCompare(dataString3, resultString) == 0);
	resultString = clarityArrayShift(array);
	assert(clarityStringCompare(dataString2, resultString) == 0);
	resultString = clarityArrayShift(array);
	assert(clarityStringCompare(dataString1, resultString) == 0);
	resultString = clarityArrayShift(array);
	assert(resultString == NULL);
	clarityArrayUnshift(array, dataString1);
	clarityArrayUnshift(array, dataString2);
	clarityArrayUnshift(array, dataString3);
	clarityArrayLock(array);
	length = clarityArrayLength(array);
	assert(clarityArrayShift(array) == NULL);
	assert(length == clarityArrayLength(array));
	clarityArrayUnshift(array, dataString3);
	assert(length == clarityArrayLength(array));
	clarityArrayUnshift(NULL, dataString3);
}

