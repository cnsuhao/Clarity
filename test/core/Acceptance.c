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

static ClarityObject *scope = NULL;

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
		clarityObjectGetOwnMember(parameters, "$1"));
	clarityObjectSetMember(scope, "data2",
		clarityObjectGetOwnMember(parameters, "$2"));

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
		clarityObjectGetOwnMember(parameters, "$1"));

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
		clarityObjectGetOwnMember(parameters, "$1"));
	clarityObjectSetMember(scope, "data2",
		clarityObjectGetOwnMember(parameters, "$2"));

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

static ClarityObject *myOtherInternalFunction(ClarityObject *parameters)
{
	ClarityCore *core = clarityCore(parameters);
	ClarityObject *scope = clarityObjectCreate(core);
	ClarityObject *functionCall;

	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data",
		clarityObjectGetOwnMember(parameters, "$1"));
	clarityObjectSetMember(scope, "callback",
		clarityObjectGetOwnMember(parameters, "$2"));

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

static ClarityObject *myThirdInternalFunction(ClarityObject *parameters)
{
	/*
     *	myThirdInternalFunction = (data, otherData) ->
	 */
	ClarityCore *core = clarityCore(parameters);
	ClarityObject *scope = clarityObjectCreate(core);

	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data",
		clarityObjectGetOwnMember(parameters, "$1"));
	clarityObjectSetMember(scope, "otherData",
		clarityObjectGetOwnMember(parameters, "$2"));

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
	clarityArrayLock((ClarityArray *)
		clarityObjectGetInnerData(clarityObjectGetMember(scope, "anArray")));
	functionCall = clarityObjectCreate(core);
	clarityObjectSetMember(functionCall, "this",
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
	if (!scope) {
		scope = clarityObjectCreate(core);

		clarityObjectSetMember(scope, "prototype", clarityGlobal(core));
		clarityObjectSetMember(scope, "this", scope);
		clarityObjectSetMember(scope, "exports", clarityObjectCreate(core));
		clarityObjectSetMember(clarityObjectGetMember(scope, "exports"),
			"mainFunction", clarityFunctionObjectCreate(core,
			mainFunction, scope));
		clarityObjectSetMember(scope, "myInternalFunction",
			clarityFunctionObjectCreate(core, myInternalFunction, scope));
		clarityObjectSetMember(scope, "myOtherInternalFunction",
			clarityFunctionObjectCreateAsync(core,
			myOtherInternalFunction, scope));
		clarityObjectSetMember(scope, "myThirdInternalFunction",
			clarityFunctionObjectCreate(core, myThirdInternalFunction, scope));
		clarityObjectSetMember(clarityObjectGetMember(scope, "exports"),
			"testEntry", clarityFunctionObjectCreate(core,
			testEntry, scope));
		clarityPushFile(core, scope);
		clarityObjectLock(scope);
	}
	return clarityObjectGetMember(scope, "exports");
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
