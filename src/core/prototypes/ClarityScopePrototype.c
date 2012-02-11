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
#include "ClarityScopePrototype.h"
#include "ClarityBooleanObject.h"
#include "ClarityStringObject.h"
#include "ClarityFunctionObject.h"

static ClarityObject *loadedFiles = NULL;
static ClarityEventLoop *gEventLoop = NULL;
static ClarityObject *gUndefined = NULL;
static ClarityObject *gFileRegistry = NULL;

void clarityScopePrototypeStaticInitializer(ClarityEventLoop *eventLoop,
	ClarityObject *undefined, ClarityObject *fileRegistry)
{
	gEventLoop = clarityHeapRetain(eventLoop);
	gUndefined = clarityHeapRetain(undefined);
	gFileRegistry = clarityHeapRetain(fileRegistry);
}

void clarityScopePrototypeStaticRelease(void)
{
	clarityHeapRelease(gUndefined);
	clarityHeapRelease(gEventLoop);
	clarityHeapRelease(gFileRegistry);
}

static ClarityObject *clarityTypeOf(ClarityObject *parameters)
{
	return clarityStringObjectCreate(clarityHeap(parameters),
		clarityObjectTypeOf(
		clarityObjectGetOwnMember(parameters, "$1")));
}

static void clarityIfEventDone(ClarityObject *parameters)
{
	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(parameters, "$1"), parameters);
}

static void clarityIfEvent(ClarityObject *parameters)
{
	ClarityObject *scope;

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(parameters, "$1"), parameters);
	scope = clarityObjectCreate(clarityHeap(parameters));
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "$1",
		clarityObjectGetOwnMember(parameters, "$2"));
	clarityEventLoopEnqueue(gEventLoop,
		(ClarityEvent)clarityIfEventDone, scope);
}

static ClarityObject *clarityIf(ClarityObject *parameters)
{
	ClarityObject *scope;
	ClarityObject *testObject = clarityObjectGetOwnMember(parameters, "$1");
	Bool test = FALSE;

	if (clarityStrCmp(
		clarityObjectTypeOf(testObject), "function") == 0) {
			testObject = clarityFunctionObjectCall(
				testObject, parameters);
	}

	if (clarityStrCmp(
		clarityObjectTypeOf(testObject), "boolean") == 0) {
			test = clarityBooleanObjectGetValue(testObject);
	}

	scope = clarityObjectCreate(clarityHeap(parameters));
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

	clarityEventLoopEnqueue(gEventLoop,
		(ClarityEvent)clarityIfEvent, scope);

	return gUndefined;
}

static ClarityObject *clarityRequire(ClarityObject *parameters)
{
	ClarityObject *retVal = gUndefined;
	ClarityObject *file = clarityObjectGetOwnMember(parameters, "$1");

	if (clarityStrCmp(clarityObjectTypeOf(file), "string") == 0) {
		ClarityObject *loadedFile;
		const char *name;

		name = clarityStringObjectGetValue(file);
		loadedFile = clarityObjectGetMember(loadedFiles, name);

		if (loadedFile == gUndefined) {
			loadedFile = clarityFunctionObjectCall(
				clarityObjectGetMember(gFileRegistry, name),
				clarityObjectCreate(clarityHeap(parameters)));
			clarityObjectSetMember(loadedFiles, name, loadedFile);
			retVal = clarityObjectGetMember(loadedFile, "exports");
		}
	}

	return retVal;
}

ClarityObject *clarityScopePrototypeCreate(ClarityHeap *heap)
{
	ClarityObject *prototype = clarityObjectCreate(heap);

	clarityObjectSetMember(prototype, "typeOf",
		clarityFunctionObjectCreate(heap,
		clarityTypeOf, gUndefined));

	clarityObjectSetMember(prototype, "if",
		clarityFunctionObjectCreate(heap,
		clarityIf, gUndefined));

	clarityObjectSetMember(prototype, "require",
		clarityFunctionObjectCreate(heap,
		clarityRequire, gUndefined));

	if (!loadedFiles)
		loadedFiles = clarityObjectCreate(heap);

	clarityObjectLock(prototype);
	return prototype;
}
