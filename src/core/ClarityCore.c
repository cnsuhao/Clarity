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
#include "Clarity.h"
#include "ClarityObject.h"
#include "ClarityArrayObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityStringObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityEventLoop.h"
#include "ClarityFileStore.h"

struct __ClarityCore {
	ClarityHeap *heap;
	ClarityEventLoop *eventLoop;
	ClarityFileStore *fileStore;
	ClarityMemCpy memCpy;
	ClarityMemSet memSet;
	ClarityStrLen strLen;
	ClarityStrCmp strCmp;
	ClarityObject *global;
	ClarityObject *arrayPrototype;
	ClarityObject *booleanPrototype;
	ClarityObject *integerPrototype;
	ClarityObject *stringPrototype;
	ClarityObject *functionPrototype;
	ClarityObject *objectPrototype;
};

static ClarityObject *undefined = NULL;

static ClarityObject *clarityUndefinedObjectCreate(ClarityCore *core)
{
	if (!undefined) {
		undefined = clarityObjectCreateType(core, "undefined", NULL);
		clarityObjectLock(undefined);
	}
	return undefined;
}

static ClarityObject *clarityTypeOf(ClarityObject *parameters)
{
	return clarityStringObjectCreate(clarityCore(parameters),
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
	ClarityCore *core = clarityCore(parameters);

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(parameters, "$1"), parameters);
	scope = clarityObjectCreate(clarityCore(parameters));
	clarityObjectSetMember(scope, "prototype", parameters);
	clarityObjectSetMember(scope, "$1",
		clarityObjectGetOwnMember(parameters, "$2"));
	clarityEnqueueEvent(core, (ClarityEvent)clarityIfEventDone, scope);
}

static ClarityObject *clarityIf(ClarityObject *parameters)
{
	ClarityObject *scope;
	ClarityCore *core = clarityCore(parameters);
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

	scope = clarityObjectCreate(clarityCore(parameters));
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

static ClarityObject *clarityGlobalObjectCreate(ClarityCore *core)
{
	ClarityObject *global = clarityObjectCreate(core);

	clarityObjectSetMember(global, "typeOf",
		clarityFunctionObjectCreate(core,
			clarityTypeOf, clarityUndefined()));

	clarityObjectSetMember(global, "if",
		clarityFunctionObjectCreate(core,
			clarityIf, clarityUndefined()));

	clarityObjectLock(global);
	return global;
}

static void *defaultMemCpy(ClarityCore *core, void *dstData,
	const void *srcData, Uint32 size)
{
	char* dst8 = (char *)dstData;
	char* src8 = (char *)srcData;

	while (size--)
		*dst8++ = *src8++;
	return dstData;
}

static void *defaultMemSet(ClarityCore *core, void *data, char value,
	Uint32 size)
{
	char *p;

	p = data;
	while (size--)
		*p++ = value;
	return data;
}

static Uint32 defaultStrLen(ClarityCore *core, const char *cString)
{
	const char *s;

	for (s = cString; *s;)
		++s;
	return (Uint32)(s - cString);
}

static Sint8 defaultStrCmp(ClarityCore *core, const char *cString,
	const char *cString2)
{
	unsigned char uc1;
	unsigned char uc2;

	while (*cString != '\0' && *cString == *cString2) {
		cString++;
		cString2++;
	}

	uc1 = (*(unsigned char *) cString);
	uc2 = (*(unsigned char *) cString2);
	return ((uc1 < uc2) ? -1 : (uc1 > uc2));
}

void claritySetMemCpy(ClarityCore *core, ClarityMemCpy memCpy)
{
	core->memCpy = memCpy;
}

void claritySetMemSet(ClarityCore *core, ClarityMemSet memSet)
{
	core->memSet = memSet;
}

void claritySetStrLen(ClarityCore *core, ClarityStrLen strLen)
{
	core->strLen = strLen;
}

void claritySetStrCmp(ClarityCore *core, ClarityStrCmp strCmp)
{
	core->strCmp = strCmp;
}

void *clarityMemCpy(ClarityCore *core, void *dstData, const void *srcData,
	Uint32 size)
{
	return core->memCpy(core, dstData, srcData, size);
}

void *clarityMemSet(ClarityCore *core, void *data, char value, Uint32 size)
{
	return core->memSet(core, data, value, size);
}

Uint32 clarityStrLen(ClarityCore *core, const char *cString)
{
	return core->strLen(core, cString);
}

Sint8 clarityStrCmp(ClarityCore *core, const char *cString,
	const char *cString2)
{
	return core->strCmp(core, cString, cString2);
}

void *clarityAllocateWithDestructor(ClarityCore *core, Uint32 size,
	ClarityDestructor destructor)
{
	return clarityHeapAllocateWithDestructor(core->heap, size,
		(ClarityHeapDestructor)destructor);
}

void *clarityAllocate(ClarityCore *core, Uint32 size)
{
	return clarityHeapAllocate(core->heap, size);
}

void *clarityAutoRelease(void *data)
{
	return clarityHeapAutoRelease(data);
}

void clarityRelease(void *data)
{
	clarityHeapRelease(data);
}

void clarityForceRelease(void *data)
{
	clarityHeapForceRelease(data);
}

void *clarityRetain(void *data)
{
	return clarityHeapRetain(data);
}

ClarityCore *clarityCore(void *data)
{
	return (ClarityCore *)clarityHeapGetContext(clarityHeapGetHeap(data));
}

void *clarityUndefined(void)
{
	return undefined;
}

void *clarityGlobal(ClarityCore *core)
{
	return core->global;
}

void clarityCollectGarbage(ClarityCore *core)
{
	clarityHeapCollectGarbage(core->heap);
}

void clarityEnqueueEvent(ClarityCore *core, ClarityEvent function, void *data)
{
	clarityEventLoopEnqueue(core->eventLoop, function, data);
}

void clarityPushEvent(ClarityCore *core, ClarityEvent function, void *data)
{
	clarityEventLoopPush(core->eventLoop, function, data);
}

void clarityPushFile(ClarityCore *core, void *file)
{
	clarityFileStorePush(core->fileStore, file);
}

static void clarityDestroy(ClarityCore *core)
{
	clarityRelease(core->eventLoop);
	clarityRelease(core->global);
	clarityRelease(core->fileStore);
	clarityRelease(core->arrayPrototype);
	clarityRelease(core->booleanPrototype);
	clarityRelease(core->integerPrototype);
	clarityRelease(core->stringPrototype);
	clarityRelease(core->functionPrototype);
	clarityForceRelease(core->objectPrototype);
	clarityRelease(core->heap);
}

ClarityCore *clarityCreate(ClarityEvent entry, ClarityHeap *heap)
{
	ClarityCore *core;

	core = clarityHeapAllocateWithDestructor(heap, sizeof(ClarityCore),
		(ClarityHeapDestructor)clarityDestroy);

	core->memSet = defaultMemSet;
	core->memCpy = defaultMemCpy;
	core->strLen = defaultStrLen;
	core->strCmp = defaultStrCmp;
	clarityHeapSetContext(heap, core);
	core->heap = clarityRetain(heap);
	core->objectPrototype = clarityRetain(
		clarityObjectPrototypeCreate(core));
	undefined = clarityRetain(clarityUndefinedObjectCreate(core));
	core->global = clarityRetain(clarityGlobalObjectCreate(core));
	core->eventLoop = clarityRetain(clarityEventLoopCreate(core, entry));
	core->fileStore = clarityRetain(clarityFileStoreCreate(core));
	core->arrayPrototype = clarityRetain(
		clarityArrayPrototypeCreate(core));
	core->booleanPrototype = clarityRetain(
		clarityBooleanPrototypeCreate(core));
	core->integerPrototype = clarityRetain(
		clarityIntegerPrototypeCreate(core));
	core->stringPrototype = clarityRetain(
		clarityStringPrototypeCreate(core));
	core->functionPrototype = clarityRetain(
		clarityFunctionPrototypeCreate(core));
	return clarityAutoRelease(core);
}

void clarityStart(ClarityCore *core)
{
	clarityRetain(core);
	clarityEventLoopStart(core->eventLoop);
}

void clarityStop(ClarityCore *core)
{
	clarityRelease(undefined);
	clarityRelease(core);
}
