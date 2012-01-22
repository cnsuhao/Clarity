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
#include "ClarityArrayObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityStringObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityEventLoop.h"

struct __ClarityCore {
	ClarityHeap *heap;
	ClarityEventLoop *eventLoop;
	ClarityObject *globalScope;
	ClarityObject *fileRegistry;
	ClarityObject *arrayPrototype;
	ClarityObject *booleanPrototype;
	ClarityObject *integerPrototype;
	ClarityObject *stringPrototype;
	ClarityObject *functionPrototype;
	ClarityObject *objectPrototype;
};

static ClarityCore *core = NULL;
static ClarityObject *undefined = NULL;

static ClarityObject *clarityUndefinedObjectCreate(ClarityCore *core)
{
	if (!undefined) {
		undefined = clarityObjectCreateType(core, "undefined", NULL);
		clarityObjectLock(undefined);
	}
	return undefined;
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

void clarityRegisterFile(ClarityCore *core, char *name, ClarityFileInit init)
{
	clarityObjectSetMember(core->fileRegistry, name,
		clarityFunctionObjectCreate(core,
		(ClarityFunctionPointer)init, core->globalScope));
}

void *clarityFileRegistry(ClarityCore *core)
{
	return core->fileRegistry;
}

void *clarityUndefined(void)
{
	return undefined;
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

static void clarityDestroy(ClarityCore *core)
{
	clarityRelease(core->eventLoop);
	clarityRelease(core->globalScope);
	clarityRelease(core->arrayPrototype);
	clarityRelease(core->booleanPrototype);
	clarityRelease(core->integerPrototype);
	clarityRelease(core->stringPrototype);
	clarityRelease(core->functionPrototype);
	clarityRelease(core->fileRegistry);
	clarityForceRelease(core->objectPrototype);
	clarityRelease(core->heap);
}

ClarityCore *clarityCreate(ClarityEvent entry, ClarityHeap *heap)
{
	core = clarityHeapAllocateWithDestructor(heap, sizeof(ClarityCore),
		(ClarityHeapDestructor)clarityDestroy);

	core->heap = clarityRetain(heap);
	core->objectPrototype = clarityRetain(
		clarityObjectPrototypeCreate(core));
	undefined = clarityRetain(clarityUndefinedObjectCreate(core));
	core->fileRegistry = clarityRetain(clarityObjectCreate(core));
	core->globalScope = clarityRetain(clarityGlobalScopeObjectCreate(core));
	core->eventLoop = clarityRetain(clarityEventLoopCreate(core,
		entry, core->globalScope));
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

ClarityObject *clarityEntry(ClarityObject *) __attribute__((weak));
ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	return clarityUndefined();
}

ClarityCore *clarityCore(void)
{
	if (!core)
		core = clarityCreate((ClarityEvent)clarityEntry,
			clarityHeapCreate());
	return core;
}

int main(void) __attribute__((weak));
int main(void)
{
	clarityStart(clarityCore());
	clarityStop(core);
	return 0;
}

