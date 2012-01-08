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

static ClarityObject *entry(ClarityObject *this, ClarityObject *upperScope)
{
	ClarityCore *core = clarityCore(this);
	ClarityObject *scope = clarityObjectCreateWithPrototype(core, upperScope);
	ClarityObject *functionCall = clarityObjectCreateWithPrototype(core, scope);
	clarityObjectSetMember(scope, "data1", clarityObjectCreate(core));
	clarityObjectSetMember(clarityObjectGetMember(scope, "data1"), "astring", clarityStringCreate(core, "hej"));
	clarityObjectSetMember(clarityObjectGetMember(scope, "data1"), "aNumber", clarityIntegerCreate(core, 23));
	clarityObjectSetMember(scope, "data2", clarityObjectCreate(core));
	clarityObjectSetMember(clarityObjectGetMember(scope, "data2"), "anObject", clarityObjectCreate(core));
	clarityObjectSetMember(clarityObjectGetMember(clarityObjectGetMember(scope, "data2"), "anObject"), "anotherString", clarityStringCreate(core, "hepp"));
	clarityObjectSetMember(clarityObjectGetMember(clarityObjectGetMember(scope, "data2"), "anObject"), "anObject", clarityObjectGetMember(scope, "data1"));
	clarityObjectSetMember(scope, "anArray", clarityArrayObjectCreate(core));
	clarityArrayUnshift(clarityObjectGetMember(scope, "anArray"), clarityStringCreate(core, "hepp"));
	clarityArrayUnshift(clarityObjectGetMember(scope, "anArray"), clarityObjectGetMember(clarityObjectGetMember(scope, "data1"), "aNumber"));
	clarityArrayUnshift(clarityObjectGetMember(scope, "anArray"), clarityIntegerCreate(core, 34));
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$0", clarityObjectGetMember(scope, "exports"));
	clarityObjectSetMember(functionCall, "$1", clarityObjectGetMember(scope, "data1"));
	clarityObjectSetMember(functionCall, "$2", clarityObjectGetMember(scope, "data2"));
	clarityFunctionObjectCall(clarityObjectGetMember(clarityObjectGetMember(scope, "exports"), "mainFunction"), functionCall);
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$0", scope);
	clarityObjectSetMember(functionCall, "$1", clarityObjectGetMember(clarityObjectGetMember(scope, "data2"), "anObject"));
	clarityObjectSetMember(functionCall, "$2", clarityObjectGetMember(scope, "anArray"));
	return clarityFunctionObjectCall(clarityObjectGetMember(scope, "myInternalFunction"), functionCall);
}

ClarityObject *testImplementationCreate(ClarityCore *core)
{
	ClarityObject *exports = NULL;
	if (file != NULL) {
		file = clarityObjectCreateWithPrototype(core, clarityGlobal(core));
		exports = clarityObjectCreate(core);
		clarityObjectSetMember(file, "exports", exports);
		clarityObjectSetMember(clarityObjectGetMember(file, "exports"), "mainFunction", clarityFunctionObjectCreate(core, mainFunction));
		clarityObjectSetMember(file, "myInternalFunction", clarityFunctionObjectCreate(core, myInternalFunction, file));
		clarityObjectSetMember(file, "myOtherInternalFunction", clarityFunctionObjectCreate(core, myOtherInternalFunction, file));
		clarityObjectSetMember(file, "myThirdInternalFunction", clarityFunctionObjectCreate(core, myThirdInternalFunction, file));
		clarityObjectSetMember(clarityObjectGetMember(file, "exports"), "mainFunction", clarityFunctionObjectCreate(core, mainFunction, file));
		clarityAddFileObject(core, file);
		clarityObjectLock(array);
	}
	return clarityObjectGetMember(file, "exports");
}
