#include "TestImplementation.h"

static ClarityObject *file = NULL;

static void *_otherFunction(void *inData, void *outData)
{
	return clarityIntegerCreate(clarityCore(inData), 23);
}

static void *mainFunction(void *inData, void *outData)
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
	ClarityObject *eventData = clarityObjectCreate(core);
	clarityObjectSetMember(eventData, "inData", inData);
	clarityObjectSetMember(eventData, "outData", outData);
	clarityObjectSetMember(eventData, "callback", callback);
	clarityEnqueueEvent(core, (ClarityEvent)myOtherInternalFunctionEvent, eventData);
	return 0;
}

static void *_myThirdInternalFunction(void *outData)
{
	return otherFunction->function(outData);
}

static void *entry(ClarityObject *scope)
{
	ClarityObject *data1 = clarityObjectCreate(core);
	ClarityObject *data2 = clarityObjectCreate(core);
	ClarityArray *anArray = clarityArrayCreate(core);

	clarityObjectSetMember(data1, "astring", clarityStringCreate(core, "hej"));
	clarityObjectSetMember(data1, "aNumber", clarityIntegerCreate(core, 23));
	clarityObjectSetMember(data2, "anObject", clarityObjectCreate(core));
	clarityObjectSetMember(clarityObjectGetMember(data2, "anObject"), "anotherString", clarityStringCreate(core, "hepp"));
	clarityObjectSetMember(clarityObjectGetMember(data2, "anObject"), "anObject", inData);
	clarityArrayUnshift(anArray, clarityStringCreate(core, "hepp"));
	clarityArrayUnshift(anArray, clarityObjectGetMember(data1, "aNumber"));
	clarityArrayUnshift(anArray, clarityIntegerCreate(core, 34));
	mainFunction->function(data1, data2);
	return myInternalFunction->function(clarityObjectGetMember(data2, "anObject"), anArray);
}

static void *testImplementation(ClarityObject *scope)
{
	ClarityCore *core = clarityCore(scope);
	clarityObjectSetMember(clarityObjectGetMember(scope, "exports"), "mainFunction", clarityFunctionObjectCreate(core, mainFunction));
	clarityObjectSetMember(scope, "myInternalFunction", clarityFunctionObjectCreate(core, myInternalFunction));
	clarityObjectSetMember(scope, "myOtherInternalFunction", clarityFunctionObjectCreate(core, myOtherInternalFunction));
	clarityObjectSetMember(scope, "myThirdInternalFunction", clarityFunctionObjectCreate(core, myThirdInternalFunction));
	clarityObjectSetMember(clarityObjectGetMember(scope, "exports"), "mainFunction", clarityFunctionObjectCreate(core, mainFunction));
}

ClarityObject *testImplementationCreate(ClarityCore *core)
{
	ClarityObject *exports = NULL;
	if (file != NULL) {
		file = clarityObjectCreate(core);
		exports = clarityObjectCreate(core);
		clarityObjectSetMember(file, "exports", exports);
		testImplementation(file);
		clarityAddFileObject(core, file);
		clarityObjectLock(array);
	}
	return exports;
}
