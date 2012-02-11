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
#include "ClarityFactory.h"
#include "ClarityCore.h"
#include "ClarityObject.h"
#include "ClarityObjectPrototype.h"
#include "ClarityArrayPrototype.h"
#include "ClarityIntegerPrototype.h"
#include "ClarityBooleanPrototype.h"
#include "ClarityStringPrototype.h"
#include "ClarityScopePrototype.h"
#include "ClarityArrayObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityStringObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityEventLoop.h"

struct __Clarity {
	ClarityHeap *heap;
	ClarityEventLoop *eventLoop;
	ClarityObject *scopePrototype;
	ClarityObject *fileRegistry;
	ClarityObject *objectPrototype;
};

static ClarityObject *undefined = NULL;
static Clarity *clarity = NULL;

static ClarityObject *clarityUndefinedObjectCreate(ClarityHeap *heap)
{
	if (!undefined) {
		undefined = clarityObjectCreateType(heap, "undefined", NULL);
		clarityObjectLock(undefined);
	}
	return undefined;
}

void clarityRegisterFile(Clarity *clarity, char *name, ClarityFileInit init)
{
	clarityObjectSetMember(clarity->fileRegistry, name,
		clarityFunctionObjectCreate(clarityHeap(clarity),
		(ClarityFunctionPointer)init, clarity->scopePrototype));
}

static void clarityDestroy(Clarity *clarity)
{
	clarityHeapForceRelease(clarity->eventLoop);
	clarityHeapForceRelease(clarity->scopePrototype);
	clarityHeapForceRelease(clarity->fileRegistry);
	clarityHeapForceRelease(clarity->objectPrototype);
	clarityHeapRelease(clarity->heap);
}

Clarity *clarityCreate(ClarityEvent entry, ClarityHeap *heap)
{
	Clarity *clarity = clarityHeapAllocateWithDestructor(heap,
		sizeof(Clarity),
		(ClarityHeapDestructor)clarityDestroy);

	clarity->heap = clarityHeapRetain(heap);
	undefined = clarityHeapRetain(clarityUndefinedObjectCreate(heap));
	clarity->fileRegistry = clarityHeapRetain(clarityObjectCreate(heap));
	clarity->scopePrototype = clarityHeapRetain(
		clarityScopePrototypeCreate(heap));
	clarity->eventLoop = clarityHeapRetain(
		clarityEventLoopCreate(clarity->heap,
		entry, clarity->scopePrototype));
	clarityObjectPrototypeStaticInitializer(undefined);
	clarity->objectPrototype = clarityObjectPrototypeCreate(heap);
	clarityObjectStaticInitializer(clarity->objectPrototype, undefined);
	clarityArrayPrototypeStaticInitializer(clarity->eventLoop, undefined);
	clarityArrayStaticInitializer(
		clarityArrayPrototypeCreate(heap), undefined);
	clarityIntegerPrototypeStaticInitializer(undefined);
	clarityIntegerStaticInitializer(
		clarityIntegerPrototypeCreate(heap), undefined);
	clarityBooleanPrototypeStaticInitializer(undefined);
	clarityBooleanStaticInitializer(
		clarityBooleanPrototypeCreate(heap), undefined);
	clarityStringStaticInitializer(
		clarityStringPrototypeCreate(heap), undefined);
	clarityScopePrototypeStaticInitializer(clarity->eventLoop,
		undefined, clarity->fileRegistry);
	clarityFunctionStaticInitializer(heap, clarity->eventLoop, undefined);
	return clarityHeapAutoRelease(clarity);
}

void clarityStart(Clarity *clarity)
{
	clarityHeapRetain(clarity);
	clarityEventLoopStart(clarity->eventLoop);
}

void clarityStop(Clarity *clarity)
{
	clarityFunctionStaticRelease();
	clarityArrayStaticRelease();
	clarityBooleanStaticRelease();
	clarityIntegerStaticRelease();
	clarityStringStaticRelease();
	clarityScopePrototypeStaticRelease();
	clarityArrayPrototypeStaticRelease();
	clarityIntegerPrototypeStaticRelease();
	clarityBooleanPrototypeStaticRelease();
	clarityStringPrototypeStaticRelease();
	clarityObjectPrototypeStaticRelease();
	clarityObjectStaticRelease();
	clarityHeapRelease(undefined);
	clarityHeapRelease(clarity);
}

ClarityObject *clarityEntry(ClarityObject *) __attribute__((weak));
ClarityObject *clarityEntry(ClarityObject *object)
{
	return undefined;
}

Clarity *clarityCore(void)
{
	if (!clarity)
		clarity = clarityCreate((ClarityEvent)clarityEntry,
			clarityHeapCreate());
	return clarity;
}

int main(void) __attribute__((weak));
int main(void)
{
	clarityStart(clarityCore());
	clarityStop(clarity);
	return 0;
}

