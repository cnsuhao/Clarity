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
#include "ClarityObjectPriv.h"
#include "ClarityObjectPrototype.h"
#include "ClarityArrayPrototype.h"
#include "ClarityNumberPrototype.h"
#include "ClarityBooleanPrototype.h"
#include "ClarityStringPrototype.h"
#include "ClarityScopePrototype.h"
#include "ClarityArrayObjectPriv.h"
#include "ClarityBooleanObjectPriv.h"
#include "ClarityNumberObjectPriv.h"
#include "ClarityStringObjectPriv.h"
#include "ClarityFunctionObjectPriv.h"
#include "ClarityEventLoop.h"

struct __Clarity {
	ClarityHeap *heap;
	ClarityEventLoop *eventLoop;
	ClarityObject *scopePrototype;
	ClarityObject *fileRegistry;
	ClarityObject *objectPrototype;
	ClarityObject *undefined;
};

static Clarity *clarity = 0;

void clarityRegisterFile(Clarity *clarity, const char *name,
	ClarityFileInit init)
{
	if (clarity)
		clarityObjectSetMember(clarity->fileRegistry, name,
			clarityFunctionObjectCreate(clarityHeap(clarity),
			(ClarityFunctionPointer)init, clarity->scopePrototype));
}

static void clarityDestroy(Clarity *clarity)
{
	if (clarity) {
		clarityHeapRelease(clarity->eventLoop);
		clarityHeapRelease(clarity->scopePrototype);
		clarityHeapRelease(clarity->fileRegistry);
		clarityHeapRelease(clarity->objectPrototype);
		clarityHeapRelease(clarity->undefined);
		clarityHeapRelease(clarity->heap);
	}
}

Clarity *clarityCreate(ClarityHeap *heap)
{
	Clarity *clarity = clarityHeapAllocateWithDestructor(heap,
		sizeof(Clarity),
		(ClarityHeapDestructor)clarityDestroy);

	if (clarity) {
		clarity->heap = clarityHeapRetain(heap);
		clarity->undefined = clarityHeapRetain(
			clarityObjectCreateType(heap, "undefined", 0));
		clarityObjectLock(clarity->undefined);
		clarity->fileRegistry = clarityHeapRetain(
			clarityObjectCreate(heap));
		clarity->scopePrototype = clarityHeapRetain(
			clarityScopePrototypeCreate(heap));
		clarity->eventLoop = clarityHeapRetain(
			clarityEventLoopCreate(clarity->heap));
		clarityObjectPrototypeStaticInitializer(clarity->undefined);
		clarity->objectPrototype = clarityObjectPrototypeCreate(heap);
		clarityObjectStaticInitializer(clarity->objectPrototype,
			clarity->undefined);
		clarityArrayPrototypeStaticInitializer(clarity->eventLoop,
			clarity->undefined);
		clarityArrayStaticInitializer(
			clarityArrayPrototypeCreate(heap), clarity->undefined);
		clarityNumberPrototypeStaticInitializer(clarity->undefined);
		clarityNumberStaticInitializer(
			clarityNumberPrototypeCreate(heap),
			clarity->undefined);
		clarityBooleanPrototypeStaticInitializer(clarity->undefined);
		clarityBooleanStaticInitializer(
			clarityBooleanPrototypeCreate(heap),
			clarity->undefined);
		clarityStringPrototypeStaticInitializer(clarity->undefined);
		clarityStringStaticInitializer(
			clarityStringPrototypeCreate(heap), clarity->undefined);
		clarityScopePrototypeStaticInitializer(clarity->eventLoop,
			clarity->undefined, clarity->fileRegistry);
		clarityFunctionStaticInitializer(clarity->eventLoop,
			clarity->undefined);
	}
	return clarityHeapAutoRelease(clarity);
}

void clarityStart(Clarity *clarity, const char *entry)
{
	if (clarity) {
		clarityHeapRetain(clarity);
		clarityFunctionObjectCall(
			clarityObjectGetMember(clarity->scopePrototype,
			"require"),
			clarityObjectSetMember(
			clarityObjectCreate(clarityHeap(clarity)), "$1",
			clarityStringObjectCreate(
			clarityHeap(clarity), entry)));
		clarityEventLoopStart(clarity->eventLoop);
	}
}

void clarityStop(Clarity *clarity)
{
	clarityFunctionStaticRelease();
	clarityArrayStaticRelease();
	clarityBooleanStaticRelease();
	clarityNumberStaticRelease();
	clarityStringStaticRelease();
	clarityScopePrototypeStaticRelease();
	clarityArrayPrototypeStaticRelease();
	clarityNumberPrototypeStaticRelease();
	clarityBooleanPrototypeStaticRelease();
	clarityStringPrototypeStaticRelease();
	clarityObjectPrototypeStaticRelease();
	clarityObjectStaticRelease();
	clarityHeapRelease(clarity);
}

Clarity *clarityCore(void)
{
	if (!clarity)
		clarity = clarityCreate(clarityHeapCreate());
	return clarity;
}

int main(void) __attribute__((weak));
int main(void)
{
	clarityStart(clarityCore(), "entry");
	clarityStop(clarity);
	return 0;
}

