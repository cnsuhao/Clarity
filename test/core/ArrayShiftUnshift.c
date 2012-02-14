#include "Clarity.h"
#include "ClarityArray.h"
#include <assert.h>
#include <string.h>

static ClarityObject *clarityEntry(ClarityObject *globalScope)
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
	clarityArrayUnshift(array, dataString1);
	clarityArrayUnshift(array, dataString2);
	clarityArrayUnshift(array, dataString3);
	resultString = clarityArrayShift(array);
	assert(strcmp(data3,
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityArrayShift(array);
	assert(strcmp(data2,
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityArrayShift(array);
	assert(strcmp(data1,
		clarityStringObjectGetValue(resultString)) == 0);
	resultString = clarityArrayShift(array);
	assert(resultString == 0);
	clarityArrayUnshift(array, dataString1);
	clarityArrayUnshift(array, dataString2);
	clarityArrayUnshift(array, dataString3);
	clarityArrayLock(array);
	length = clarityArrayLength(array);
	assert(clarityArrayShift(array) == 0);
	assert(length == clarityArrayLength(array));
	clarityArrayUnshift(array, dataString3);
	assert(length == clarityArrayLength(array));
	clarityArrayUnshift(0, dataString3);
	clarityArrayLock(0);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
