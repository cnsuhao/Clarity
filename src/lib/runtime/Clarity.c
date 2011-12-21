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
#include "ClarityEventLoop.h"

struct __Clarity {
	ClarityHeap *heap;
	ClarityEventLoop *eventLoop;
	ClarityMemCpy memCpy;
	ClarityMemSet memSet;
	ClarityStrLen strLen;
};

static void *defaultMemCpy(void *dstData, const void *srcData, Uint32 size)
{
	char* dst8 = (char *)dstData;
	char* src8 = (char *)srcData;

	while (size--)
		*dst8++ = *src8++;
	return dstData;
}

static void *defaultMemSet(void *data, char value, Uint32 size)
{
	char *p;

	p = data;
	while (size--)
		*p++ = value;
	return data;
}

static Uint32 defaultStrLen(const char *string)
{
	const char *s;

	for (s = string; *s;)
		++s;
	return (Uint32)(s - string);
}

void claritySetMemCpy(Clarity *clarity, ClarityMemCpy memCpy)
{
	clarity->memCpy = memCpy;
}

void claritySetMemSet(Clarity *clarity, ClarityMemSet memSet)
{
	clarity->memSet = memSet;
}

void ClaritySetStrLen(Clarity *clarity, ClarityStrLen strLen)
{
	clarity->strLen = strLen;
}

ClarityMemCpy clarityGetMemCpy(Clarity *clarity)
{
	return clarity->memCpy;
}

ClarityMemSet clarityGetMemSet(Clarity *clarity)
{
	return clarity->memSet;
}

ClarityStrLen ClarityGetStrLen(Clarity *clarity)
{
	return clarity->strLen;
}

ClarityHeap *clarityGetHeap(Clarity *clarity)
{
	return clarity->heap;
}

void clarityEnqueueEvent(Clarity *clarity,
						ClarityEventFunction function,
						void *data)
{
	clarityEventLoopEnqueue(clarity->eventLoop, function, data);
}

static void destroy(ClarityHeap *heap, Clarity *clarity)
{
	clarityHeapRelease(heap, clarity->eventLoop);
	clarityHeapRelease(heap, clarity->heap);
}

Clarity *clarityCreate(ClarityEventFunction entry, ClarityHeap *heap)
{
	Clarity *clarity;

	clarity = clarityHeapAllocate(heap,
								  sizeof(Clarity),
								  (ClarityHeapDestructor)destroy);

	clarity->memSet = defaultMemSet;
	clarity->memCpy = defaultMemCpy;
	clarity->strLen = defaultStrLen;
	clarity->heap = clarityHeapRetain(heap, heap);
	clarity->eventLoop = clarityEventLoopCreate(clarity, entry);
	clarity->eventLoop = clarityHeapRetain(heap, clarity->eventLoop);
	clarityHeapAutoRelease(heap, clarity);
	return clarity;
}

void clarityStart(Clarity *clarity)
{
	clarityEventLoopStart(clarity->eventLoop);
}

void clarityStop(Clarity *clarity)
{
	clarityHeapRelease(clarity->heap, clarity->eventLoop);
	clarityHeapRelease(clarity->heap, clarity->heap);
}
