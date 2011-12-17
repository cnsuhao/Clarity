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
};

ClarityHeap *clarityGetHeap(Clarity *clarity)
{
	return clarity->heap;
}

void clarityPushEvent(Clarity *clarity,
					  ClarityEventFunction function,
					  void *data)
{
	clarityEventLoopPush(clarity->eventLoop, function, data);
}

static void destroy(ClarityHeap *heap, Clarity *clarity)
{
	clarityHeapRelease(heap, clarity->eventLoop);
	clarityHeapRelease(heap, clarity->heap);
}

Clarity *clarityCreate(ClarityEventFunction entry)
{
	Clarity *clarity;
	ClarityHeap *heap;

	heap = clarityHeapCreate();
	clarity = clarityHeapAllocate(heap,
								  sizeof(Clarity),
								  (ClarityHeapDestructor)destroy);

	clarity->heap = clarityHeapRetain(heap, heap);
	clarity->eventLoop = clarityEventLoopCreate(clarity, entry);
	clarity->eventLoop = clarityHeapRetain(heap, clarity->eventLoop);
	clarityEventLoopStart(clarity->eventLoop);
	clarityHeapAutoRelease(heap, clarity);
	return clarity;
}
