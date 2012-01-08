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
#include "ClarityString.h"
#include "ClarityObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityFunctionObject.h"

static ClarityObject *equals(ClarityObject *scope)
{
	ClarityObject *retVal = clarityUndefined();

	if (scope) {
		ClarityCore *core = clarityCore(scope);

		if (clarityStrCmp(core, clarityObjectTypeOf(
			clarityObjectGetMember(scope, "$0")), "string") == 0 &&
			clarityStrCmp(core, clarityObjectTypeOf(
			clarityObjectGetMember(scope, "$1")), "string") == 0) {
			Uint32 compare = clarityStringCompare(
				clarityObjectGetInnerData(clarityObjectGetMember(scope, "$0")),
				clarityObjectGetInnerData(clarityObjectGetMember(scope, "$1")));

			retVal = clarityBooleanObjectCreate(core, compare == 0);
		}
	}
	return retVal;
}

static ClarityObject *length(ClarityObject *scope)
{
	ClarityObject *retVal = clarityUndefined();

	if (scope) {
		ClarityCore *core = clarityCore(scope);

		if (clarityStrCmp(core, clarityObjectTypeOf(
		clarityObjectGetMember(scope, "$0")), "string") == 0) {
			Uint32 length = clarityStringLength(
				clarityObjectGetInnerData(clarityObjectGetMember(scope, "$0")));

			retVal = clarityIntegerObjectCreate(core, length);
		}
	}
	return retVal;
}

ClarityObject *clarityStringObjectCreate(ClarityCore *core,
	const char *cString)
{
	ClarityObject *string;

	string = clarityObjectCreateType(core, "string",
		clarityStringCreate(core, cString));

	clarityObjectSetMember(string, "equals",
		clarityFunctionObjectCreate(core,
			equals,
			clarityUndefined()));

	clarityObjectSetMember(string, "length",
		clarityFunctionObjectCreate(core,
			length,
			clarityUndefined()));

	clarityObjectLock(string);
	return string;
}
