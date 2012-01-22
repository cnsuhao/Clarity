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
#include "ClarityCore.h"
#include "ClarityObject.h"
#include "ClarityGlobalScopeObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityBoolean.h"
#include "ClarityStringObject.h"
#include "ClarityString.h"
#include "ClarityFunctionObject.h"

static ClarityObject *globalScope = NULL;
static ClarityObject *loadedFiles = NULL;

static ClarityObject *clarityTypeOf(ClarityObject *parameters)
{
	return clarityStringObjectCreate(clarityCore(),
		clarityObjectTypeOf(clarityObjectGetOwnMember(parameters, "$1")));
}

static void clarityIfEventDone(ClarityObject *parameters)
{
	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(parameters, "$1"), parameters);
}

static void clarityIfEvent(ClarityObject *parameters)
{
	ClarityObject *scope;
	ClarityCore *core = clarityCore();

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(parameters, "$1"), parameters);
	scope = clarityObjectCreate(clarityCore());
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "$1",
		clarityObjectGetOwnMember(parameters, "$2"));
	clarityEnqueueEvent(core, (ClarityEvent)clarityIfEventDone, scope);
}

static ClarityObject *clarityIf(ClarityObject *parameters)
{
	ClarityObject *scope;
	ClarityCore *core = clarityCore();
	ClarityObject *testObject = clarityObjectGetOwnMember(parameters, "$1");
	Bool test = FALSE;

	if (clarityStrCmp(core,
		clarityObjectTypeOf(testObject), "function") == 0) {
			testObject = clarityFunctionObjectCall(testObject, parameters);
	}

	if (clarityStrCmp(core,
		clarityObjectTypeOf(testObject), "boolean") == 0) {
			test = clarityBooleanGetValue(
					clarityObjectGetInnerData(testObject));
	}

	scope = clarityObjectCreate(clarityCore());
	clarityObjectSetMember(scope, "prototype", parameters);

	if (test) {
		clarityObjectSetMember(scope, "$1",
			clarityObjectGetOwnMember(parameters, "$2"));
	} else {
		clarityObjectSetMember(scope, "$1",
			clarityObjectGetOwnMember(parameters, "$3"));
	}
	clarityObjectSetMember(scope, "$2",
		clarityObjectGetOwnMember(parameters, "$4"));
	clarityEnqueueEvent(core, (ClarityEvent)clarityIfEvent, scope);
	return clarityUndefined();
}

static ClarityObject *clarityRequire(ClarityObject *parameters)
{
	ClarityCore *core = clarityCore();
	ClarityObject *retVal = clarityUndefined();
	ClarityObject *file = clarityObjectGetOwnMember(parameters, "$1");

	if (clarityStrCmp(core,
		clarityObjectTypeOf(file), "string") == 0) {
			ClarityObject *loadedFile;
			const char *name;

			name = clarityStringGetValue(clarityObjectGetInnerData(file));
			loadedFile = clarityObjectGetMember(loadedFiles, name);
			if (loadedFile == clarityUndefined()) {
				loadedFile = clarityFunctionObjectCall(
					clarityObjectGetMember(
					(ClarityObject *)clarityFileRegistry(core), name),
					clarityObjectCreate(core));
				clarityObjectSetMember(loadedFiles, name, loadedFile);
				retVal = clarityObjectGetMember(loadedFile, "exports");
			}
	}

	return retVal;
}

ClarityObject *clarityGlobalScopeObjectCreate(ClarityCore *core)
{
	if (!globalScope) {
		globalScope = clarityObjectCreate(core);

		clarityObjectSetMember(globalScope, "typeOf",
			clarityFunctionObjectCreate(core,
				clarityTypeOf, clarityUndefined()));

		clarityObjectSetMember(globalScope, "if",
			clarityFunctionObjectCreate(core,
				clarityIf, clarityUndefined()));

		clarityObjectSetMember(globalScope, "require",
			clarityFunctionObjectCreate(core,
				clarityRequire, clarityUndefined()));

		if (!loadedFiles)
			loadedFiles = clarityObjectCreate(core);

		clarityObjectLock(globalScope);
	}
	return globalScope;
}
