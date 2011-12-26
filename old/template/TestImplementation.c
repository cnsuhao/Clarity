#include "TestImplementation.h"

void mainFunction(Clarity *clarity, SomeInData *inData, SomeOutData *outData)
{
	\Uint16\data;
	someFunction(clarity, *data);
	myInternalFunction(clarity, inData, outData);
	otherFunction(clarity, outData);
}

static void myInternalFunction_myOtherInternalFunctionFunction$1(Clarity *clarity, SomeOutData *returnData)
{
	OtherFunction(clarity, returnData);
}

void myInternalFunction(Clarity *clarity, SomeInData *inData, SomeOutData *outData)
{
	myOtherInternalFunction(clarity, inData, outData, myInternalFunction_myOtherInternalFunction$1);
}

typedef struct
{
	SomeInData *inData;
	SomeOutData *outData;
	FuncType *callback;
} myOtherInternalFunction$Struct$1;

static void myOtherInternalFunction$Internal$1(Clarity *clarity, SomeInData *inData, SomeOutData *outData, FuncType *callback)
{
	myThirdInternalFunction(clarity, outData);
	otherFunction(clarity, outData);
	callback->func(clarity, outData);
}

static void myInternalFunction_myOtherInternalFunction$Event$1(Clarity *clarity, myOtherInternalFunction$Struct$1 *eventData)
{
	myOtherInternalFunction$Internal$1(clarity, eventData->inData, eventData->outData, eventData->callback);
	clarityRelease(clarity, eventData);
	clarityRelease(clarity, eventData->inData);
	clarityRelease(clarity, eventData->outData);
	clarityRelease(clarity, eventData->callback);
}

void myOtherInternalFunction(clarity,SomeInData *inData, SomeOutData *outData, FuncType *callback)
{
	myOtherInternalFunction$Struct$1 *eventData = clarityAlloc(clarity, sizeof(myOtherInternalFunction$Struct$1));
	eventData->inData = clarityRetain(clarity, inData);
	eventData->outData = clarityRetain(clarity, outData);
	eventData->callback = clarityRetain(clarity, callback);
	clarityPushEvent(clarity, myInternalFunction_myOtherInternalFunction$Event$1, eventData);
}

void myThirdInternalFunction(SomeOutData *outData)
{
	otherFunction(outData);
}

void entry(Clarity *clarity)
{
	\SomeInData\inData = \SomeInData\;
	\SomeOutData\outData = \SomeOutData\;
	mainFunction(clarity, inData, outData);
	myInternalFunction(clarity, inData, outData);
}
