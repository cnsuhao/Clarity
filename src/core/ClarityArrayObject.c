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

static ClarityObject *length(ClarityObject *array, ClarityObject *context)
{
	ClarityObject *retVal = clarityUndefined();

	if (array) {
		ClarityCore *core = clarityCore(array);

		if (clarityStrCmp(core, clarityObjectTypeOf(array), "array")) {
			Uint32 length = clarityArrayLength(
				clarityObjectGetInnerData(array));

			retVal = clarityIntegerObjectCreate(core, length);
		}
	}
	return retVal;
}

static void forEachFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));

	clarityObjectSetMember(context, "prototype", upperContext);
	clarityObjectSetMember(context, "$1", item);

	clarityObjectSetMember(context, "$2",
		clarityIntegerObjectCreate(clarityCore(item), index));

	clarityObjectSetMember(context, "$3",
		clarityArrayObjectCreate(clarityCore(item), array));

	clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$1"), context);
}

static void *mapFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));

	clarityObjectSetMember(context, "prototype", upperContext);
	clarityObjectSetMember(context, "$1", item);

	clarityObjectSetMember(context, "$2",
		clarityIntegerObjectCreate(clarityCore(item), index));

	clarityObjectSetMember(context, "$3",
		clarityArrayObjectCreate(clarityCore(item), array));

	return clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$1"), context);
}

static Bool testFunction(ClarityObject *item,
	Uint32 index, ClarityArray *array, ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));

	clarityObjectSetMember(context, "prototype", upperContext);

	clarityObjectSetMember(context, "$1", item);

	clarityObjectSetMember(context, "$2",
		clarityIntegerObjectCreate(clarityCore(item), index));

	clarityObjectSetMember(context, "$3",
		clarityArrayObjectCreate(clarityCore(item), array));

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

	clarityObjectSetMember(context, "prototype", upperContext);
	clarityObjectSetMember(context, "$1",
		clarityArrayObjectCreate(clarityCore(upperContext), newArray));

	clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$2"), upperContext);
}

static void testCallback(Bool test,
	ClarityObject *upperContext)
{
	ClarityObject *context = clarityObjectCreate(clarityCore(upperContext));

	clarityObjectSetMember(context, "prototype", upperContext);
	clarityObjectSetMember(context, "$1",
		clarityBooleanObjectCreate(clarityCore(upperContext), test));

	clarityFunctionObjectCall(
		clarityObjectGetMember(upperContext, "$2"), upperContext);
}

static ClarityObject *forEach(ClarityObject *array,
	ClarityObject *context)
{
	clarityArrayForEach(clarityObjectGetInnerData(array),
		(ClarityArrayForEachFunction)forEachFunction,
		(ClarityArrayForEachCallback)forEachCallback,
		context);
	return clarityUndefined();
}

static ClarityObject *map(ClarityObject *array,
	ClarityObject *context)
{
	clarityArrayMap(clarityObjectGetInnerData(array),
		(ClarityArrayMapFunction)mapFunction,
		(ClarityArrayMapCallback)mapCallback,
		context);
	return clarityUndefined();
}

static  ClarityObject *every(ClarityObject *array,
	ClarityObject *context)
{
	clarityArrayEvery(clarityObjectGetInnerData(array),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayTestCallback)testCallback,
		context);
	return clarityUndefined();
}

static ClarityObject *some(ClarityObject *array,
	ClarityObject *context)
{
	clarityArraySome(clarityObjectGetInnerData(array),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayTestCallback)testCallback,
		context);
	return clarityUndefined();
}

static  ClarityObject *filter(ClarityObject *array,
	ClarityObject *context)
{
	clarityArrayFilter(clarityObjectGetInnerData(array),
		(ClarityArrayTestFunction)testFunction,
		(ClarityArrayMapCallback)mapCallback,
		context);
	return clarityUndefined();
}

ClarityObject *clarityArrayObjectCreate(ClarityCore *core,
	ClarityArray *innerArray)
{
	ClarityObject *array = clarityObjectCreateType(core, "array", innerArray);

	array = clarityObjectCreate(core);

	clarityObjectSetMember(array, "length",
		clarityFunctionObjectCreate(core,
			(ClarityFunctionPointer)length, clarityUndefined()));

	clarityObjectSetMember(array, "forEach",
		clarityFunctionObjectCreate(core,
			(ClarityFunctionPointer)forEach, clarityUndefined()));

	clarityObjectSetMember(array, "map",
		clarityFunctionObjectCreate(core,
			(ClarityFunctionPointer)map, clarityUndefined()));

	clarityObjectSetMember(array, "every",
		clarityFunctionObjectCreate(core,
			(ClarityFunctionPointer)every, clarityUndefined()));

	clarityObjectSetMember(array, "some",
		clarityFunctionObjectCreate(core,
			(ClarityFunctionPointer)some, clarityUndefined()));

	clarityObjectSetMember(array, "filter",
		clarityFunctionObjectCreate(core,
			(ClarityFunctionPointer)filter, clarityUndefined()));

	clarityObjectLock(array);
	return array;
}
