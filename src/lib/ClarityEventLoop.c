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
#include "ClarityEventLoop.h"
#include "ClarityArray.h"

typedef struct {
	ClarityEventFunction function;
	void *data;
	Clarity *clarity;
} Event;

struct __ClarityEventLoop {
	ClarityArray *array;
	Clarity *clarity;
	ClarityEventFunction entry;
};

static void eventDestroy(ClarityHeap *heap, Event *event)
{
	clarityHeapRelease(heap, event);
}

static Event *eventCreate(Clarity *clarity,
						  ClarityEventFunction function,
						  void *data)
{
	Event *event;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	event = clarityHeapAllocate(heap, sizeof(Event),
								(ClarityHeapDestructor)eventDestroy);

	event->clarity = clarityHeapRetain(heap, clarity);
	event->data = clarityHeapRetain(heap, data);
	event->function = function;
	clarityHeapAutoRelease(heap, event);
	return event;
}

static Bool clarityEventLoopHasEvent(ClarityEventLoop *eventLoop)
{
	return clarityArrayLength(eventLoop->array);
}

static void destroy(ClarityHeap *heap,
					ClarityEventLoop *eventLoop)
{
	clarityHeapRelease(heap, eventLoop->clarity);
	clarityHeapRelease(heap, eventLoop->array);
}

static void clarityEventLoopPop(ClarityEventLoop *eventLoop)
{
	Event *event;
	ClarityHeap *heap;

	heap = clarityGetHeap(eventLoop->clarity);
	event = clarityArrayPop(eventLoop->array);
	event->function(eventLoop->clarity, event->data);
	clarityHeapRelease(heap, event->data);
	clarityHeapRelease(heap, event);
	clarityHeapCollectGarbage(heap);
}

void clarityEventLoopPush(ClarityEventLoop *eventLoop,
						  ClarityEventFunction function,
						  void *data)
{
	Event *event;
	ClarityHeap *heap;

	heap = clarityGetHeap(eventLoop->clarity);
	event = eventCreate(eventLoop->clarity, function, data);
	event = clarityHeapRetain(heap, event);
	clarityArrayPush(eventLoop->array, event);
}

void clarityEventLoopStart(ClarityEventLoop *eventLoop)
{
	eventLoop->entry(eventLoop->clarity, NULL);

	while (clarityEventLoopHasEvent(eventLoop))
		clarityEventLoopPop(eventLoop);
}

ClarityEventLoop *clarityEventLoopCreate(Clarity *clarity,
										 ClarityEventFunction entry)
{
	ClarityHeap *heap;
	ClarityEventLoop *eventLoop;

	heap = clarityGetHeap(clarity);
	eventLoop = clarityHeapAllocate(heap,
									sizeof(ClarityEventLoop),
									(ClarityHeapDestructor)destroy);

	eventLoop->clarity = clarityHeapRetain(heap, clarity);
	eventLoop->array = clarityArrayCreate(eventLoop->clarity);
	eventLoop->array = clarityHeapRetain(heap, eventLoop->array);
	eventLoop->entry = entry;
	clarityHeapAutoRelease(heap, eventLoop);
	return eventLoop;
}
