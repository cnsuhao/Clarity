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
#include "ClarityIntegerObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityFunctionObject.h"

static ClarityObject *gPrototype = NULL;
static ClarityObject *gUndefined = NULL;

void clarityIntegerStaticInitializer(ClarityObject *prototype,
	ClarityObject *undefined)
{
	gUndefined = clarityHeapRetain(undefined);
	gPrototype = clarityHeapRetain(prototype);
}

void clarityIntegerStaticRelease(void)
{
	clarityHeapRelease(gUndefined);
	clarityHeapRelease(gPrototype);
}

typedef struct {
	Uint32 value;
} ClarityInteger;

static ClarityInteger *clarityIntegerCreate(ClarityHeap *heap, Uint32 value)
{
	ClarityInteger *integer;

	integer = clarityHeapAllocate(heap, sizeof(ClarityInteger));
	if (integer)
		integer->value = value;

	return clarityHeapAutoRelease(integer);
}

Uint32 clarityIntegerObjectGetValue(ClarityObject *integer)
{
	Uint32 retVal = 0;
	if (clarityObjectIsTypeOf(integer, "number")) {
		ClarityInteger *inner;

		inner = (ClarityInteger *)
			clarityObjectGetInnerData(integer);

		if (inner)
			retVal = inner->value;
	}
	return retVal;
}

ClarityObject *clarityIntegerObjectCreate(ClarityHeap *heap, Uint32 uint32)
{
	ClarityObject *integer;

	integer = clarityObjectCreateType(heap, "number",
		clarityIntegerCreate(heap, uint32));

	clarityObjectSetMember(integer, "prototype", gPrototype);

	clarityObjectLock(integer);
	return integer;
}
