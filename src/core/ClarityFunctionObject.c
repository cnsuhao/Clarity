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
} ClarityFunction;

static ClarityFunction *clarityFunctionCreate(ClarityCore *core,
	ClarityFunctionPointer functionPointer)
{
	ClarityFunction *function;

	function = clarityAllocate(core, sizeof(ClarityFunction));

	function->functionPointer = functionPointer;
	return clarityAutoRelease(function);
}

ClarityObject *clarityFunctionObjectCreate(
	ClarityCore *core, ClarityFunctionPointer functionPointer)
{
	ClarityObject *function;

	function = clarityObjectCreateType(core, "function",
		clarityFunctionCreate(core, functionPointer));

	return function;
}

ClarityObject *clarityFunctionObjectCall(ClarityObject *function,
	ClarityObject *scope)
{
	ClarityObject *retVal = clarityObjectUndefined(clarityCore(function));

	if (function && scope) {
		ClarityFunction *inner;

		inner = (ClarityFunction *)clarityObjectGetInnerData(function);

		if (inner) {
			ClarityFunctionPointer functionPointer;

			functionPointer = inner->functionPointer;

			if (functionPointer)
				retVal = functionPointer(scope);
		}
	}
	return retVal;
}
