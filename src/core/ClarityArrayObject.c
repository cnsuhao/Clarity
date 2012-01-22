/*
 * Copyright 2011 Patchwork Solutions AB. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *      1. Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 *
 *      2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY Patchwork Solutions AB ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL Patchwork Solutions AB OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Patchwork Solutions AB.
 */
#include "ClarityArray.h"
#include "ClarityArrayObject.h"
#include "ClarityObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityBoolean.h"
#include "ClarityBooleanObject.h"
#include "ClarityFunctionObject.h"

static ClarityObject *prototype = NULL;

static ClarityObject *length(ClarityObject *scope)
{
	ClarityObject *retVal = clarityUndefined();

	if (scope) {
		ClarityCore *core = clarityCore();

		if (clarityStrCmp(core, clarityObjectTypeOf(
		clarityObjectGetMember(scope, "this")), "array") == 0) {
			Uint32 length = clarityArrayLength(
				clarityObjectGetInnerData(clarityObjectGetMember(scope,
				"this")));

			retVal = clarityIntegerObjectCreate(core, length);
		}
	}
	return retVal;
}

static void forEachFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *parameters)
{
	ClarityObject *scope = clarityObjectCreate(clarityCore());

	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "$1", item);

	clarityObjectSetMember(scope, "$2",
		clarityIntegerObjectCreate(clarityCore(), index));

	clarityObjectSetMember(scope, "$3",
		clarityObjectGetMember(parameters, "this"));

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(parameters, "$1"), scope);
}

static void *mapFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *parameters)
{
	ClarityObject *scope = clarityObjectCreate(clarityCore());

	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "$1", item);

	clarityObjectSetMember(scope, "$2",
		clarityIntegerObjectCreate(clarityCore(), index));

	clarityObjectSetMember(scope, "$3",
		clarityObjectGetMember(parameters, "this"));

	return clarityFunctionObjectCall(
		clarityObjectGetOwnMember(parameters, "$1"), scope);
}

static Bool testFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *parameters)
{
	ClarityObject *scope = clarityObjectCreate(clarityCore());

	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "$1", item);

	clarityObjectSetMember(scope, "$2",
		clarityIntegerObjectCreate(clarityCore(), index));

	clarityObjectSetMember(scope, "$3",
		clarityObjectGetMember(parameters, "this"));

	return clarityBooleanGetValue((ClarityBoolean *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(
		parameters, "$1"), scope)));
}

static void forEachCallback(ClarityObject *upperContext)
{
	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(upperContext, "$2"), upperContext);
}

static void mapCallback(ClarityArray *newArray,
	ClarityObject *upperContext)
{
	ClarityObject *scope = clarityObjectCreate(clarityCore());

	clarityObjectSetMember(scope, "$1",
		clarityArrayObjectCreate(clarityCore(), newArray));

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(upperContext, "$2"), scope);
}

static void testCallback(Bool test,
	ClarityObject *upperContext)
{
	ClarityObject *scope = clarityObjectCreate(clarityCore());

	clarityObjectSetMember(scope, "$1",
		clarityBooleanObjectCreate(clarityCore(), test));

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(upperContext, "$2"), scope);
}

static ClarityObject *forEach(ClarityObject *scope)
{
	clarityArrayForEach(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		(ClarityArrayForEachFunction)forEachFunction,
		(ClarityArrayForEachCallback)forEachCallback,
		scope);
	return clarityUndefined();
}

static ClarityObject *map(ClarityObject *scope)
{
	clarityArrayMap(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		(ClarityArrayMapFunction)mapFunction,
		(ClarityArrayMapCallback)mapCallback,
		scope);
	return clarityUndefined();
}

static  ClarityObject *every(ClarityObject *scope)
{
	clarityArrayEvery(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayTestCallback)testCallback,
		scope);
	return clarityUndefined();
}

static ClarityObject *some(ClarityObject *scope)
{
	clarityArraySome(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayTestCallback)testCallback,
		scope);
	return clarityUndefined();
}

static  ClarityObject *filter(ClarityObject *scope)
{
	clarityArrayFilter(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayMapCallback)mapCallback,
		scope);
	return clarityUndefined();
}

ClarityObject *clarityArrayPrototypeCreate(ClarityCore *core)
{
	if (!prototype) {
		prototype = clarityObjectCreate(core);

		clarityObjectSetMember(prototype, "length",
			clarityFunctionObjectCreate(core,
				length, clarityUndefined()));

		clarityObjectSetMember(prototype, "forEach",
			clarityFunctionObjectCreate(core,
				forEach, clarityUndefined()));

		clarityObjectSetMember(prototype, "map",
			clarityFunctionObjectCreate(core,
				map, clarityUndefined()));

		clarityObjectSetMember(prototype, "every",
			clarityFunctionObjectCreate(core,
				every, clarityUndefined()));

		clarityObjectSetMember(prototype, "some",
			clarityFunctionObjectCreate(core,
				some, clarityUndefined()));

		clarityObjectSetMember(prototype, "filter",
			clarityFunctionObjectCreate(core,
				filter, clarityUndefined()));

		clarityObjectLock(prototype);
	}
	return prototype;
}

ClarityObject *clarityArrayObjectCreate(ClarityCore *core,
	ClarityArray *innerArray)
{
	ClarityObject *array = clarityObjectCreateType(core, "array", innerArray);

	clarityObjectSetMember(array, "prototype",
		clarityArrayPrototypeCreate(core));

	clarityObjectLock(array);
	return array;
}
