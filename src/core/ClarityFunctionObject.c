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

typedef struct {
	ClarityFunctionPointer functionPointer;
	ClarityObject *scope;
} ClarityFunction;

static ClarityObject *prototype = NULL;

ClarityObject *clarityFunctionPrototypeCreate(ClarityCore *core)
{
	if (!prototype) {
		prototype = clarityObjectCreate(core);
		clarityObjectLock(prototype);
	}
	return prototype;
}

static void destroyClarityFunction(ClarityFunction *clarityFunction)
{
	clarityRelease(clarityFunction->scope);
}

static ClarityFunction *clarityFunctionCreate(ClarityCore *core,
	ClarityFunctionPointer functionPointer, ClarityObject *scope)
{
	ClarityFunction *function;

	function = clarityAllocateWithDestructor(core, sizeof(ClarityFunction),
		(ClarityDestructor)destroyClarityFunction);

	function->functionPointer = functionPointer;
	function->scope = clarityRetain(scope);
	return clarityAutoRelease(function);
}

ClarityObject *clarityFunctionObjectCreate(ClarityCore *core,
	ClarityFunctionPointer functionPointer, ClarityObject *scope)
{
	ClarityObject *function;

	function = clarityObjectCreateType(core, "function",
		clarityFunctionCreate(core, functionPointer, scope));

	clarityObjectSetMember(function, "prototype",
		clarityFunctionPrototypeCreate(core));

	return function;
}

ClarityObject *clarityFunctionObjectCall(ClarityObject *function,
	ClarityObject *parameters)
{
	ClarityObject *retVal = clarityUndefined();

	if (function && parameters) {
		ClarityCore *core = clarityCore(function);

		if (clarityStrCmp(core, clarityObjectTypeOf(function),
			"function") == 0) {
			ClarityFunction *inner;

			inner = (ClarityFunction *)clarityObjectGetInnerData(function);

			if (inner) {
				ClarityFunctionPointer functionPointer;

				functionPointer = inner->functionPointer;
				clarityObjectSetMember(parameters, "prototype",
					inner->scope);
				clarityObjectLock(parameters);

				if (functionPointer)
					retVal = functionPointer(parameters);
			}
		}
	}
	return retVal;
}