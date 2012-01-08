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

static ClarityObject *length(ClarityObject *scope)
{
	ClarityObject *retVal = clarityUndefined();

	if (scope) {
		ClarityCore *core = clarityCore(scope);

		if (clarityStrCmp(core, clarityObjectTypeOf(
		clarityObjectGetMember(scope, "$0")), "array") == 0) {
			Uint32 length = clarityArrayLength(
				clarityObjectGetInnerData(clarityObjectGetMember(scope, "$0")));

			retVal = clarityIntegerObjectCreate(core, length);
		}
	}
	return retVal;
}

static void forEachFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));

	clarityObjectSetMember(context, "$1", item);

	clarityObjectSetMember(context, "$2",
		clarityIntegerObjectCreate(clarityCore(item), index));

	clarityObjectSetMember(context, "$3",
		clarityObjectGetMember(upperContext, "$0"));

	clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$1"), context);
}

static void *mapFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));

	clarityObjectSetMember(context, "$1", item);

	clarityObjectSetMember(context, "$2",
		clarityIntegerObjectCreate(clarityCore(item), index));

	clarityObjectSetMember(context, "$3",
		clarityObjectGetMember(upperContext, "$0"));

	return clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$1"), context);
}

static Bool testFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));


	clarityObjectSetMember(context, "$1", item);

	clarityObjectSetMember(context, "$2",
		clarityIntegerObjectCreate(clarityCore(item), index));

	clarityObjectSetMember(context, "$3",
		clarityObjectGetMember(upperContext, "$0"));

	return clarityBooleanGetValue((ClarityBoolean *)clarityObjectGetInnerData(
		clarityFunctionObjectCall(clarityObjectGetMember(
		upperContext, "$1"), context)));
}

static void forEachCallback(ClarityObject *upperContext)
{
	clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$2"), upperContext);
}

static void mapCallback(ClarityArray *newArray,
	ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));

	clarityObjectSetMember(context, "$1",
		clarityArrayObjectCreate(clarityCore(upperContext), newArray));

	clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$2"), context);
}

static void testCallback(Bool test,
	ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));

	clarityObjectSetMember(context, "$1",
		clarityBooleanObjectCreate(clarityCore(upperContext), test));

	clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$2"), context);
}

static ClarityObject *forEach(ClarityObject *scope)
{
	clarityArrayForEach(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$0")),
		(ClarityArrayForEachFunction)forEachFunction,
		(ClarityArrayForEachCallback)forEachCallback,
		scope);
	return clarityUndefined();
}

static ClarityObject *map(ClarityObject *scope)
{
	clarityArrayMap(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$0")),
		(ClarityArrayMapFunction)mapFunction,
		(ClarityArrayMapCallback)mapCallback,
		scope);
	return clarityUndefined();
}

static  ClarityObject *every(ClarityObject *scope)
{
	clarityArrayEvery(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$0")),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayTestCallback)testCallback,
		scope);
	return clarityUndefined();
}

static ClarityObject *some(ClarityObject *scope)
{
	clarityArraySome(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$0")),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayTestCallback)testCallback,
		scope);
	return clarityUndefined();
}

static  ClarityObject *filter(ClarityObject *scope)
{
	clarityArrayFilter(clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "$0")),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayMapCallback)mapCallback,
		scope);
	return clarityUndefined();
}

ClarityObject *clarityArrayObjectCreate(ClarityCore *core,
	ClarityArray *innerArray)
{
	ClarityObject *array = clarityObjectCreateType(core, "array", innerArray);

	clarityObjectSetMember(array, "length",
		clarityFunctionObjectCreate(core,
			length, clarityUndefined()));

	clarityObjectSetMember(array, "forEach",
		clarityFunctionObjectCreate(core,
			forEach, clarityUndefined()));

	clarityObjectSetMember(array, "map",
		clarityFunctionObjectCreate(core,
			map, clarityUndefined()));

	clarityObjectSetMember(array, "every",
		clarityFunctionObjectCreate(core,
			every, clarityUndefined()));

	clarityObjectSetMember(array, "some",
		clarityFunctionObjectCreate(core,
			some, clarityUndefined()));

	clarityObjectSetMember(array, "filter",
		clarityFunctionObjectCreate(core,
			filter, clarityUndefined()));

	clarityObjectLock(array);
	return array;
}
