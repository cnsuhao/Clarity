#include "TestImplementation.h"

static void *_otherFunction(void *inData, void *outData);
static const ClarityFunction __otherFunction = {.function = (ClarityFunctionPointer)_otherFunction};
static const ClarityFunction *otherFunction = &__otherFunction;

static void *_myInternalFunction(void *inData, void *outData);
static const ClarityFunction __myInternalFunction = {.function = (ClarityFunctionPointer)_myInternalFunction};
static const ClarityFunction *myInternalFunction = &__myInternalFunction;

static void *_myOtherInternalFunction(void *inData, void *outData, void *callback);
static const ClarityFunction __myOtherInternalFunction = {.function = (ClarityFunctionPointer)_myOtherInternalFunction};
static const ClarityFunction *myOtherInternalFunction = &__myOtherInternalFunction;

static void *_myThirdInternalFunction(void *outData);
static const ClarityFunction __myThirdInternalFunction = {.function = (ClarityFunctionPointer)_myThirdInternalFunction};
static const ClarityFunction *myThirdInternalFunction = &__myThirdInternalFunction;

static void *_entry(ClarityCore *clarity);
static const ClarityFunction __entry = {.function = (ClarityFunctionPointer)_entry};
const ClarityFunction *entry = &__entry;

static void *_mainFunction(void *data1, void *data2);
static const ClarityFunction __mainFunction = {.function = (ClarityFunctionPointer)_mainFunction};
const ClarityFunction *mainFunction = &__mainFunction;

static void *_otherFunction(void *inData, void *outData)
{
	return clarityIntegerCreate(clarityCore(inData), 23);
}

static void *_mainFunction(void *inData, void *outData)
{
	return myInternalFunction->function(inData, outData);
}

static void myInternalFunction$Anonymous$1(void *returnData)
{
	otherFunction->function(returnData);
}

static void *_myInternalFunction(void *inData, void *outData)
{
	return myOtherInternalFunction->function(inData, outData, myInternalFunction$Anonymous$1);
}

static void myOtherInternalFunctionEvent(ClarityObject *eventData)
{
	void *inData = clarityObjectGetMember(eventData, "inData");
	void *outData = clarityObjectGetMember(eventData, "outData");
	void *callback = clarityObjectGetMember(eventData, "callback");
	myThirdInternalFunction->function(outData);
	otherFunction->function(outData);
	((ClarityFunction*)callback)->function(inData, outData);
}

static void *_myOtherInternalFunction(void *inData, void *outData, void *callback)
{
	ClarityCore *clarity = clarityCore(inData);
	ClarityObject *eventData = clarityObjectCreate(clarity);
	clarityObjectSetMember(eventData, "inData", inData);
	clarityObjectSetMember(eventData, "outData", outData);
	clarityObjectSetMember(eventData, "callback", callback);
	clarityEnqueueEvent(clarity, (ClarityEvent)myOtherInternalFunctionEvent, eventData);
	return 0;
}

static void *_myThirdInternalFunction(void *outData)
{
	return otherFunction->function(outData);
}

static void *_entry(ClarityCore *clarity)
{
	ClarityObject *inData = clarityObjectCreate(clarity);
	ClarityObject *outData = clarityObjectCreate(clarity);
	ClarityObject *anObject = clarityObjectCreate(clarity);
	ClarityArray *anArray = clarityArrayCreate(clarity);

	clarityObjectSetMember(inData, "astring", clarityStringCreate(clarity, "hej"));
	clarityObjectSetMember(inData, "aNumber", clarityIntegerCreate(clarity, 23));
	clarityObjectSetMember(outData, "anObject", anObject);
	clarityObjectSetMember(anObject, "anotherString", clarityStringCreate(clarity, "hepp"));
	clarityObjectSetMember(anObject, "anObject", inData);
	clarityArrayUnshift(anArray, clarityStringCreate(clarity, "hepp"));
	clarityArrayUnshift(anArray, clarityIntegerCreate(clarity, 23));
	clarityArrayUnshift(anArray, clarityIntegerCreate(clarity, 33));
	mainFunction->function(inData, outData);
	return myInternalFunction->function(inData, outData);
}
