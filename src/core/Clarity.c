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
	ClarityStrCmp strCmp;
};

static void *defaultMemCpy(Clarity *clarity,
						   void *dstData,
						   const void *srcData,
						   Uint32 size)
{
	char* dst8 = (char *)dstData;
	char* src8 = (char *)srcData;

	while (size--)
		*dst8++ = *src8++;
	return dstData;
}

static void *defaultMemSet(Clarity *clarity,
						   void *data,
						   char value,
						   Uint32 size)
{
	char *p;

	p = data;
	while (size--)
		*p++ = value;
	return data;
}

static Uint32 defaultStrLen(Clarity *clarity, const char *cString)
{
	const char *s;

	for (s = cString; *s;)
		++s;
	return (Uint32)(s - cString);
}

static Sint8 defaultStrCmp(Clarity *clarity,
						   const char *cString,
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

void claritySetMemCpy(Clarity *clarity, ClarityMemCpy memCpy)
{
	clarity->memCpy = memCpy;
}

void claritySetMemSet(Clarity *clarity, ClarityMemSet memSet)
{
	clarity->memSet = memSet;
}

void claritySetStrLen(Clarity *clarity, ClarityStrLen strLen)
{
	clarity->strLen = strLen;
}

void claritySetStrCmp(Clarity *clarity, ClarityStrCmp strCmp)
{
	clarity->strCmp = strCmp;
}


void *clarityMemCpy(Clarity *clarity,
					void *dstData,
					const void *srcData,
					Uint32 size)
{
	return clarity->memCpy(clarity, dstData, srcData, size);
}

void *clarityMemSet(Clarity *clarity, void *data, char value, Uint32 size)
{
	return clarity->memSet(clarity, data, value, size);
}

Uint32 clarityStrLen(Clarity *clarity, const char *cString)
{
	return clarity->strLen(clarity, cString);
}

Sint8 clarityStrCmp(Clarity *clarity,
					const char *cString,
					const char *cString2)
{
	return clarity->strCmp(clarity, cString, cString2);
}

void *clarityAllocate(Clarity *clarity,
					  Uint32 size,
					  ClarityDestructor destructor)
{
	return clarityHeapAllocate(clarity->heap,
							   clarity,
							   size,
							   (ClarityHeapDestructor)destructor);
}

void *clarityAutoRelease(void *data)
{
	return clarityHeapAutoRelease(data);
}

void clarityRelease(void *data)
{
	clarityHeapRelease(data);
}

void *clarityRetain(void *data)
{
	return clarityHeapRetain(data);
}

Clarity *clarity(void *data)
{
	return (Clarity *)clarityHeapGetContext(data);
}

void clarityCollectGarbage(Clarity *clarity)
{
	clarityHeapCollectGarbage(clarity->heap);
}

void clarityEnqueueEvent(Clarity *clarity,
						 ClarityEvent function,
						 void *data)
{
	clarityEventLoopEnqueue(clarity->eventLoop, function, data);
}

void clarityPushEvent(Clarity *clarity,
					  ClarityEvent function,
					  void *data)
{
	clarityEventLoopPush(clarity->eventLoop, function, data);
}

static void clarityDestroy(Clarity *clarity)
{
	clarityRelease(clarity->eventLoop);
	clarityRelease(clarity->heap);
}

Clarity *clarityCreate(ClarityEvent entry, ClarityHeap *heap)
{
	Clarity *clarity;

	clarity = clarityHeapAllocate(heap,
								  NULL,
								  sizeof(Clarity),
								  (ClarityHeapDestructor)clarityDestroy);

	clarity->memSet = defaultMemSet;
	clarity->memCpy = defaultMemCpy;
	clarity->strLen = defaultStrLen;
	clarity->strCmp = defaultStrCmp;
	clarity->heap = clarityRetain(heap);
	clarity->eventLoop = clarityEventLoopCreate(clarity, entry);
	clarity->eventLoop = clarityRetain(clarity->eventLoop);
	return clarityAutoRelease(clarity);
}

void clarityStart(Clarity *clarity)
{
	clarityRetain(clarity);
	clarityEventLoopStart(clarity->eventLoop);
}

void clarityStop(Clarity *clarity)
{
	ClarityHeap *heap = clarity->heap;
	clarityRelease(clarity);
	clarityHeapRelease(heap);
}
