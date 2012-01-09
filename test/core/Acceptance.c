/*
 *	exports.mainFunction = (data1, data2) ->
 *		myInternalFunction data1, data2
 *
 *	myInternalFunction = (data1, data2) ->
 *		myOtherInternalFunction data1, (returnData) ->
 *			myThirdInternalFunction returnData, data2
 *
 *	myOtherInternalFunction = (data, callback) =>
 *		myThirdInternalFunction data, 3
 *		callback data
 *
 *	myThirdInternalFunction = (data, otherData) ->
 *		34
 *
 *	exports.testEntry = ->
 *		data1 =
 *			aString: "hej"
 *			aNumber: 23
 *		data2 =
 *			anObject:
 *				anotherString: "hepp"
 *				anObject: data1
 *		anArray =
 *			"hej"
 *			data1.aNumber
 *			data1.aString.length()
 *		exports.mainFunction data1, data2
 *		myInternalFunction data2.anObject, anArray
 */
#include "Clarity.h"
#include "ClarityHeap.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static ClarityObject *file = NULL;

static ClarityObject *mainFunction(ClarityObject *parameters)
{
	/*
	 *	exports.mainFunction = (data1, data2) ->
	 */
	ClarityCore *core = clarityCore(parameters);
	ClarityObject *scope = clarityObjectCreate(core);
	ClarityObject *functionCall;
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data1",
		clarityObjectGetMember(parameters, "$1"));
	clarityObjectSetMember(scope, "data2",
		clarityObjectGetMember(parameters, "$2"));

	/*
	 *		myInternalFunction data1, data2
	 */
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$1",
		clarityObjectGetMember(scope, "data1"));
	clarityObjectSetMember(functionCall, "$2",
		clarityObjectGetMember(scope, "data2"));
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myInternalFunction"), functionCall);
}

static ClarityObject *myInternalFunction$Anonymous$1(ClarityObject *parameters)
{
	/*
	 *		(returnData) ->
	 */
	ClarityCore *core = clarityCore(parameters);
	ClarityObject *scope = clarityObjectCreate(core);
	ClarityObject *functionCall;
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "returnData",
		clarityObjectGetMember(parameters, "$1"));

	/*
	 *			myThirdInternalFunction returnData, data2
	 */
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$1",
		clarityObjectGetMember(scope, "returnData"));
	clarityObjectSetMember(functionCall, "$2",
		clarityObjectGetMember(scope, "data2"));
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myThirdInternalFunction"),
		functionCall);
}

static ClarityObject *myInternalFunction(ClarityObject *parameters)
{
	/*
	 *	myInternalFunction = (data1, data2) ->
	 */
	ClarityCore *core = clarityCore(parameters);
	ClarityObject *scope = clarityObjectCreate(core);
	ClarityObject *functionCall;
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data1",
		clarityObjectGetMember(parameters, "$1"));
	clarityObjectSetMember(scope, "data2",
		clarityObjectGetMember(parameters, "$2"));

	/*
	 *		myOtherInternalFunction data1, (returnData) ->
	 *			myThirdInternalFunction returnData, data2
	 */
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$1",
		clarityObjectGetMember(scope, "data1"));
	clarityObjectSetMember(functionCall, "$2",
		clarityFunctionObjectCreate(core,
		myInternalFunction$Anonymous$1, scope));
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myOtherInternalFunction"),
		functionCall);
}

static ClarityObject *myOtherInternalFunction$Event(ClarityObject *scope)
{
	ClarityCore *core = clarityCore(scope);
	ClarityObject *functionCall;
	/*
	 *		myThirdInternalFunction data, 3
	 */
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$1",
		clarityObjectGetMember(scope, "data"));
	clarityObjectSetMember(functionCall, "$2",
		clarityIntegerObjectCreate(core, 3));
	clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myThirdInternalFunction"),
		functionCall);

	/*
	 *		callback data
	 */
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$1",
		clarityObjectGetMember(scope, "data"));
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "callback"),
		functionCall);
}

static ClarityObject *myOtherInternalFunction(ClarityObject *parameters)
{
	/*
	 *	myOtherInternalFunction = (data, callback) =>
	 */
	ClarityCore *core = clarityCore(parameters);
	ClarityObject *scope = clarityObjectCreate(core);

	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data",
		clarityObjectGetMember(parameters, "$1"));
	clarityObjectSetMember(scope, "callback",
		clarityObjectGetMember(parameters, "$2"));
	clarityEnqueueEvent(core,
		(ClarityEvent)myOtherInternalFunction$Event, scope);
	return 0;
}

static ClarityObject *myThirdInternalFunction(ClarityObject *parameters)
{
	/*
     *	myThirdInternalFunction = (data, otherData) ->
	 */
	ClarityCore *core = clarityCore(parameters);
	ClarityObject *scope = clarityObjectCreate(core);

	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data",
		clarityObjectGetMember(parameters, "$1"));
	clarityObjectSetMember(scope, "otherData",
		clarityObjectGetMember(parameters, "$2"));

	/*
	 *		34
	 */
	return clarityIntegerObjectCreate(core, 43);
}

static ClarityObject *testEntry(ClarityObject *parameters)
{
	/*
	 *	exports.testEntry = ->
	 */
	ClarityCore *core = clarityCore(parameters);
	ClarityObject *scope = clarityObjectCreate(core);
	ClarityObject *functionCall;
	clarityObjectSetMember(scope, "prototype", parameters);

	/*
	 *		data1 =
	 *			aString: "hej"
	 *			aNumber: 23
	 */
	clarityObjectSetMember(scope, "data1", clarityObjectCreate(core));
	clarityObjectSetMember(clarityObjectGetMember(scope, "data1"),
		"aString", clarityStringObjectCreate(core, "hej"));
	clarityObjectSetMember(clarityObjectGetMember(scope, "data1"),
		"aNumber", clarityIntegerObjectCreate(core, 23));

	/*
	 *		data2 =
	 *			anObject:
	 *				anotherString: "hepp"
	 *				anObject: data1
	 */
	clarityObjectSetMember(scope, "data2", clarityObjectCreate(core));
	clarityObjectSetMember(clarityObjectGetMember(scope, "data2"),
		"anObject", clarityObjectCreate(core));
	clarityObjectSetMember(clarityObjectGetMember(
		clarityObjectGetMember(scope, "data2"), "anObject"),
		"anotherString", clarityStringObjectCreate(core, "hepp"));
	clarityObjectSetMember(clarityObjectGetMember(
		clarityObjectGetMember(scope, "data2"), "anObject"),
		"anObject", clarityObjectGetMember(scope, "data1"));

	/*
	 *		anArray =
	 *			"hej"
	 *			data1.aNumber
	 *			data1.aString.length()
	 */
	clarityObjectSetMember(scope, "anArray", clarityArrayObjectCreate(core,
		 clarityArrayCreate(core)));
	clarityArrayUnshift((ClarityArray *)
		clarityObjectGetInnerData(clarityObjectGetMember(scope, "anArray")),
		clarityStringCreate(core, "hepp"));
	clarityArrayUnshift((ClarityArray *)
		clarityObjectGetInnerData(clarityObjectGetMember(scope, "anArray")),
		clarityObjectGetMember(
		clarityObjectGetMember(scope, "data1"), "aNumber"));
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$0",
		clarityObjectGetMember(
		clarityObjectGetMember(scope, "data1"), "aString"));
	clarityArrayUnshift(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "anArray")),
		clarityFunctionObjectCall(clarityObjectGetMember(
		clarityObjectGetMember(clarityObjectGetMember(scope, "data1"),
		"aString"), "length"), functionCall));

	/*
	 *		exports.mainFunction data1, data2
	 */
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$1",
		clarityObjectGetMember(scope, "data1"));
	clarityObjectSetMember(functionCall, "$2",
		clarityObjectGetMember(scope, "data2"));
	clarityFunctionObjectCall(clarityObjectGetMember(
		clarityObjectGetMember(scope, "exports"), "mainFunction"),
		functionCall);

	/*
	 *		myInternalFunction data2.anObject, anArray
	 */
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "$1",
		clarityObjectGetMember(
		clarityObjectGetMember(scope, "data2"), "anObject"));
	clarityObjectSetMember(functionCall, "$2",
		clarityObjectGetMember(scope, "anArray"));
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myInternalFunction"),
		functionCall);
}

ClarityObject *testImplementationCreate(ClarityCore *core)
{
	ClarityObject *exports = NULL;
	if (!file) {
		file = clarityObjectCreate(core);

		clarityObjectSetMember(file, "prototype", clarityGlobal(core));
		clarityObjectSetMember(file, "$0", file);
		exports = clarityObjectCreate(core);
		clarityObjectSetMember(file, "exports", exports);
		clarityObjectSetMember(clarityObjectGetMember(file, "exports"),
			"mainFunction", clarityFunctionObjectCreate(core,
			mainFunction, file));
		clarityObjectSetMember(file, "myInternalFunction",
			clarityFunctionObjectCreate(core, myInternalFunction, file));
		clarityObjectSetMember(file, "myOtherInternalFunction",
			clarityFunctionObjectCreate(core, myOtherInternalFunction, file));
		clarityObjectSetMember(file, "myThirdInternalFunction",
			clarityFunctionObjectCreate(core, myThirdInternalFunction, file));
		clarityObjectSetMember(clarityObjectGetMember(file, "exports"),
			"testEntry", clarityFunctionObjectCreate(core,
			testEntry, file));
		clarityPushFile(core, file);
		clarityObjectLock(file);
	}
	return clarityObjectGetMember(file, "exports");
}

static void *mainAlloc(Uint32 size)
{
	return malloc(size);
}

static void mainFree(void *data)
{
	free(data);
}

static void entry(ClarityCore *core)
{
	ClarityObject *scope = testImplementationCreate(core);
	ClarityObject *functionCall = clarityObjectCreate(core);
	clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "testEntry"),
		functionCall);
}

int main(void)
{
	ClarityHeap *heap;
	ClarityCore *core;

	heap = clarityHeapCreateExternal(mainAlloc, mainFree);
	core = clarityCreate((ClarityEvent)entry, heap);
	clarityStart(core);
	clarityStop(core);
	return 0;
}
