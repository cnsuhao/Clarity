/*
 * Copyright 2012 Patchwork Solutions AB. All rights reserved.
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
#include "ClarityFunctionObject.h"
#include "ClarityEventLoop.h"
#include "ClarityObjectPriv.h"
#include "ClarityCore.h"

typedef struct {
	ClarityFunctionPointer functionPointer;
	ClarityObject *scope;
	Bool async;
} ClarityFunction;

static ClarityEventLoop *gEventLoop = 0;
static ClarityObject *gUndefined = 0;

void clarityFunctionStaticInitializer(ClarityEventLoop *eventLoop,
	ClarityObject *undefined)
{
	gEventLoop = clarityHeapRetain(eventLoop);
	gUndefined = clarityHeapRetain(undefined);
}

void clarityFunctionStaticRelease(void)
{
	clarityHeapRelease(gUndefined);
	clarityHeapRelease(gEventLoop);
}

static void destroyClarityFunction(ClarityFunction *clarityFunction)
{
	if (clarityFunction)
		clarityHeapRelease(clarityFunction->scope);
}

static ClarityFunction *clarityFunctionCreate(ClarityHeap *heap,
	ClarityFunctionPointer functionPointer,
	ClarityObject *scope, Bool async)
{
	ClarityFunction *function;

	function = clarityHeapAllocateWithDestructor(heap,
		sizeof(ClarityFunction),
		(ClarityHeapDestructor)destroyClarityFunction);

	if (function) {
		function->functionPointer = functionPointer;
		function->scope = clarityHeapRetain(scope);
		function->async = async;
	}

	return clarityHeapAutoRelease(function);
}

static void callAsyncEvent(ClarityObject *parameters)
{
	ClarityObject *function;
	ClarityFunction *inner;

	function = clarityObjectGetOwnMember(parameters, "$0");
	inner = (ClarityFunction *)clarityObjectGetInnerData(function);
	if (inner) {
		clarityObjectSetOwnMember(parameters, "prototype",
				inner->scope);
		clarityObjectLock(parameters);
		inner->functionPointer(parameters);
	}
}

ClarityObject *clarityFunctionObjectCall(ClarityObject *function,
	ClarityObject *parameters)
{
	ClarityObject *retVal = gUndefined;

	if (clarityObjectIsTypeOf(function, "function")) {
		ClarityFunction *inner;

		inner = (ClarityFunction *)clarityObjectGetInnerData(function);

		if (inner && inner->functionPointer) {
			clarityObjectSetOwnMember(parameters, "$0", function);
			clarityObjectSetOwnMember(parameters, "prototype",
				inner->scope);
			clarityObjectLock(parameters);

			if (inner->async) {
				clarityEventLoopEnqueue(gEventLoop,
					(ClarityEvent)callAsyncEvent,
					parameters);
				retVal = gUndefined;
			} else {
				retVal = inner->functionPointer(parameters);
			}
		}
	}
	return retVal;
}

ClarityObject *clarityFunctionObjectNew(ClarityObject *function,
	ClarityObject *parameters)
{
	ClarityObject *object = clarityObjectCreate(clarityHeap(function));

	clarityObjectSetOwnMember(parameters, "this", object);
	clarityFunctionObjectCall(function, parameters);
	return clarityObjectLock(object);
}

static ClarityObject *innerFunctionObjectCreate(ClarityHeap *heap,
	ClarityFunctionPointer functionPointer,
	ClarityObject *scope, Bool async)
{
	return clarityObjectCreateType(heap, "function",
		clarityFunctionCreate(heap, functionPointer, scope, async));
}

ClarityObject *clarityFunctionObjectCreateAsync(ClarityHeap *heap,
	ClarityFunctionPointer functionPointer, ClarityObject *scope)
{
	return innerFunctionObjectCreate(heap, functionPointer, scope, 1);
}

ClarityObject *clarityFunctionObjectCreate(ClarityHeap *heap,
	ClarityFunctionPointer functionPointer, ClarityObject *scope)
{
	return innerFunctionObjectCreate(heap, functionPointer,
		scope, 0);
}
