#include "Clarity.h"
#include "ClarityString.h"
#include "ClarityObject.h"
#include <stdlib.h>
#include <stdio.h>

static Uint32 count = 1;

static void printString(Clarity *clarity, void *data)
{
	count++;
	UNUSED(data);
	printf("The String: %s %d\n",
		   clarityStringGetCString((ClarityString *)data), count);

	if (count < 10000) {
		clarityEnqueueEvent(clarity,
							printString,
							clarityStringCreate(clarity, "printA"));

		clarityEnqueueEvent(clarity,
							printString,
							clarityStringCreate(clarity, "printB"));

		clarityEnqueueEvent(clarity,
							printString,
							clarityStringCreate(clarity, "printC"));
	}
}

static void entry(Clarity *clarity, void *data)
{
	UNUSED(data);
	clarityEnqueueEvent(clarity,
						printString,
						clarityStringCreate(clarity, "entry"));
	/*ClarityObject *object;
	ClarityString *string;
	ClarityString *memberTestValidate;
	ClarityString *memberTestData;
	ClarityString *memberTestData2;
	ClarityString *memberTestName;

	UNUSED(data);
	memberTestName = clarityStringCreate(clarity, "otherMember");
	memberTestData = clarityStringCreate(clarity, "data");
	memberTestData2 = clarityStringCreate(clarity, "data2");
	object = clarityObjectCreate(clarity);
	clarityObjectSetMember(object,
						   clarityStringCreate(clarity, "aMember"),
						   clarityStringCreate(clarity, "data"));

	clarityObjectSetMember(object,
						   memberTestName,
						   memberTestData);

	memberTestValidate =
		(ClarityString *)clarityObjectGetMember(object, memberTestName);

	if (clarityStringCompare(memberTestValidate, memberTestData) == 0) {
		clarityObjectSetMember(object,
							   memberTestName,
							   memberTestData2);
		memberTestValidate =
			(ClarityString *)clarityObjectGetMember(object, memberTestName);

		if (clarityStringCompare(memberTestValidate, memberTestData2) == 0) {
			string = clarityStringCreate(clarity, "string to auto release");

			if (clarityStringLength(string) == 22) {
				clarityEnqueueEvent(clarity,
									printString,
									clarityStringCreate(clarity, "entry"));
			}
		}
	}*/
}

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
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
