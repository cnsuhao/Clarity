/*
 *	testDep = require "hej"
 *
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
 *
 *	exports.testEntry()
 *
 */
#include "Clarity.h"
#include "ClarityHeap.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static ClarityObject *fileScope = NULL;

static ClarityObject *mainFunction(ClarityObject *parameters)
{
	/*
	 *	exports.mainFunction = (data1, data2) ->
	 */
	ClarityCore *core = clarityCore();
	ClarityObject *scope = clarityObjectCreate(core);
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data1",
		clarityObjectGetOwnMember(parameters, "$1"));
	clarityObjectSetMember(scope, "data2",
		clarityObjectGetOwnMember(parameters, "$2"));

	/*
	 *		myInternalFunction data1, data2
	 */
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myInternalFunction"),
		clarityObjectSetMember(
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityObjectGetMember(scope, "data1")), "$2",
		clarityObjectGetMember(scope, "data2")));
}

static ClarityObject *myInternalFunction$Anonymous$1(ClarityObject *parameters)
{
	/*
	 *		(returnData) ->
	 */
	ClarityCore *core = clarityCore();
	ClarityObject *scope = clarityObjectCreate(core);
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "returnData",
		clarityObjectGetOwnMember(parameters, "$1"));

	/*
	 *			myThirdInternalFunction returnData, data2
	 */
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myThirdInternalFunction"),
		clarityObjectSetMember(
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityObjectGetMember(scope, "returnData")), "$2",
		clarityObjectGetMember(scope, "data2")));
}

static ClarityObject *myInternalFunction(ClarityObject *parameters)
{
	/*
	 *	myInternalFunction = (data1, data2) ->
	 */
	ClarityCore *core = clarityCore();
	ClarityObject *scope = clarityObjectCreate(core);
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data1",
		clarityObjectGetOwnMember(parameters, "$1"));
	clarityObjectSetMember(scope, "data2",
		clarityObjectGetOwnMember(parameters, "$2"));

	/*
	 *		myOtherInternalFunction data1, (returnData) ->
	 *			myThirdInternalFunction returnData, data2
	 */
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myOtherInternalFunction"),
		clarityObjectSetMember(
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityObjectGetMember(scope, "data1")), "$2",
		clarityFunctionObjectCreate(core,
		myInternalFunction$Anonymous$1, scope)));
}

static ClarityObject *myOtherInternalFunction(ClarityObject *parameters)
{
	ClarityCore *core = clarityCore();
	ClarityObject *scope = clarityObjectCreate(core);

	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "data",
		clarityObjectGetOwnMember(parameters, "$1"));
	clarityObjectSetMember(scope, "callback",
		clarityObjectGetOwnMember(parameters, "$2"));

	/*
	 *		myThirdInternalFunction data, 3
	 */
	clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myThirdInternalFunction"),
		clarityObjectSetMember(
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityObjectGetMember(scope, "data")), "$2",
		clarityIntegerObjectCreate(core, 3)));

	/*
	 *		callback data
	 */
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "callback"),
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityObjectGetMember(scope, "data")));
}

static ClarityObject *myThirdInternalFunction(ClarityObject *parameters)
{
	/*
     *	myThirdInternalFunction = (data, otherData) ->
	 */
	ClarityCore *core = clarityCore();
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
	ClarityCore *core = clarityCore();
	ClarityObject *scope = clarityObjectCreate(core);
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
	clarityObjectSetMember(scope, "anArray",
		clarityArrayObjectCreate(core,
		clarityArrayLock(clarityArrayUnshift(
		clarityArrayUnshift(
		clarityArrayUnshift(
		clarityArrayCreate(core),
		clarityStringCreate(core, "hej")),
		clarityObjectGetMember(
		clarityObjectGetMember(scope, "data1"), "aNumber")),
		clarityFunctionObjectCall(clarityObjectGetMember(
		clarityObjectGetMember(clarityObjectGetMember(scope, "data1"),
		"aString"), "length"),
		clarityObjectSetMember(clarityObjectCreate(core), "this",
		clarityObjectGetMember(
		clarityObjectGetMember(scope, "data1"), "aString")))))));

	/*
	 *		exports.mainFunction data1, data2
	 */
	clarityFunctionObjectCall(clarityObjectGetMember(
		clarityObjectGetMember(scope, "exports"), "mainFunction"),
		clarityObjectSetMember(
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityObjectGetMember(scope, "data1")), "$2",
		clarityObjectGetMember(scope, "data2")));

	/*
	 *		myInternalFunction data2.anObject, anArray
	 */
	return clarityFunctionObjectCall(
		clarityObjectGetMember(scope, "myInternalFunction"),
		clarityObjectSetMember(
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityObjectGetMember(
		clarityObjectGetMember(scope, "data2"), "anObject")), "$2",
		clarityObjectGetMember(scope, "anArray")));
}

static ClarityObject *create(ClarityObject *globalScope)
{
	if (!fileScope) {
		ClarityCore *core = clarityCore();

		fileScope = clarityObjectCreate(core);
		clarityObjectSetMember(fileScope, "prototype", globalScope);
		clarityObjectSetMember(fileScope, "exports",
			clarityObjectCreate(core));
		clarityObjectSetMember(fileScope, "testDep",
			clarityFunctionObjectCall(
			clarityObjectGetMember(fileScope, "require"),
			clarityObjectSetMember(clarityObjectCreate(core), "$1",
			clarityStringObjectCreate(core, "hej"))));
		clarityObjectSetMember(
			clarityObjectGetMember(fileScope, "exports"),
			"mainFunction", clarityFunctionObjectCreate(core,
			mainFunction, fileScope));
		clarityObjectSetMember(fileScope, "myInternalFunction",
			clarityFunctionObjectCreate(core,
			myInternalFunction, fileScope));
		clarityObjectSetMember(fileScope, "myOtherInternalFunction",
			clarityFunctionObjectCreateAsync(core,
			myOtherInternalFunction, fileScope));
		clarityObjectSetMember(fileScope, "myThirdInternalFunction",
			clarityFunctionObjectCreate(core,
			myThirdInternalFunction, fileScope));
		clarityObjectSetMember(
			clarityObjectGetMember(fileScope, "exports"),
			"testEntry", clarityFunctionObjectCreate(core,
			testEntry, fileScope));
		clarityFunctionObjectCall(
			clarityObjectGetMember(
			clarityObjectGetMember(fileScope, "exports"), "testEntry"),
			clarityObjectCreate(core));
	}
	return fileScope;
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)create);
}

ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityCore *core = clarityCore();

	return clarityFunctionObjectCall(
		clarityObjectGetMember(globalScope, "require"),
		clarityObjectSetMember(clarityObjectCreate(core), "$1",
		clarityStringObjectCreate(core, CLARITY_FILE)));
}
