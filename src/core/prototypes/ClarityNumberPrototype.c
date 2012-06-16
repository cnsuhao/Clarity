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
#include "ClarityObjectPriv.h"
#include "ClarityNumberObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityFunctionObject.h"

static ClarityObject *gUndefined = 0;

void clarityNumberPrototypeStaticInitializer(ClarityObject *undefined)
{
	gUndefined = clarityHeapRetain(undefined);
}

void clarityNumberPrototypeStaticRelease(void)
{
	clarityHeapRelease(gUndefined);
}

static Bool validateNumberFunction(ClarityObject *scope)
{
	return clarityObjectIsTypeOf(
		clarityObjectGetMember(scope, "this"), "number") &&
		clarityObjectIsTypeOf(
		clarityObjectGetOwnMember(scope, "$1"), "number");
}

static ClarityObject *add(ClarityObject *scope)
{
	ClarityObject *retVal = gUndefined;

	if (validateNumberFunction(scope)) {
		Number number;

		number = (clarityNumberObjectGetValue(
			clarityObjectGetMember(scope, "this"))) +
			clarityNumberObjectGetValue(
			clarityObjectGetOwnMember(scope, "$1"));

		retVal = clarityNumberObjectCreate(clarityHeap(scope), number);
	}
	return retVal;
}

static ClarityObject *subtract(ClarityObject *scope)
{
	ClarityObject *retVal = gUndefined;

	if (validateNumberFunction(scope)) {
		Number number;

		number = (clarityNumberObjectGetValue(
			clarityObjectGetMember(scope, "this"))) -
			clarityNumberObjectGetValue(
			clarityObjectGetOwnMember(scope, "$1"));

		retVal = clarityNumberObjectCreate(clarityHeap(scope), number);
	}
	return retVal;
}


static ClarityObject *multiply(ClarityObject *scope)
{
	ClarityObject *retVal = gUndefined;

	if (validateNumberFunction(scope)) {
		Number number;

		number = (clarityNumberObjectGetValue(
			clarityObjectGetMember(scope, "this"))) *
			clarityNumberObjectGetValue(
			clarityObjectGetOwnMember(scope, "$1"));

		retVal = clarityNumberObjectCreate(clarityHeap(scope), number);
	}
	return retVal;
}


static ClarityObject *divide(ClarityObject *scope)
{
	ClarityObject *retVal = gUndefined;

	if (validateNumberFunction(scope)) {
		Number number;

		number = (clarityNumberObjectGetValue(
			clarityObjectGetMember(scope, "this"))) /
			clarityNumberObjectGetValue(
			clarityObjectGetOwnMember(scope, "$1"));

		retVal = clarityNumberObjectCreate(clarityHeap(scope), number);
	}
	return retVal;
}

static ClarityObject *equals(ClarityObject *scope)
{
	ClarityObject *retVal = gUndefined;

	if (validateNumberFunction(scope)) {
		Bool equal;

		equal = (clarityNumberObjectGetValue(
			clarityObjectGetMember(scope, "this"))) ==
			clarityNumberObjectGetValue(
			clarityObjectGetOwnMember(scope, "$1"));

		retVal = clarityBooleanObjectCreate(clarityHeap(scope), equal);
	}

	return retVal;
}

ClarityObject *clarityNumberPrototypeCreate(ClarityHeap *heap)
{
	ClarityObject *prototype = clarityObjectCreate(heap);

	clarityObjectSetOwnMember(prototype, "+",
		clarityFunctionObjectCreate(heap,
		add,
		gUndefined));
	clarityObjectSetOwnMember(prototype, "-",
		clarityFunctionObjectCreate(heap,
		subtract,
		gUndefined));
	clarityObjectSetOwnMember(prototype, "*",
		clarityFunctionObjectCreate(heap,
		multiply,
		gUndefined));
	clarityObjectSetOwnMember(prototype, "/",
		clarityFunctionObjectCreate(heap,
		divide,
		gUndefined));
	clarityObjectSetOwnMember(prototype, "equals",
		clarityFunctionObjectCreate(heap,
		equals,
		gUndefined));

	return clarityObjectLock(prototype);
}

