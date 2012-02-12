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
#include "ClarityHeap.h"

typedef struct {
	ClarityEvent function;
	void *data;
} Event;

struct __ClarityEventLoop {
	ClarityArray *events;
};

static void eventDestroy(Event *event)
{
	if (event)
		clarityHeapRelease(event->data);
}

static Event *eventCreate(ClarityHeap *heap, ClarityEvent function,
	void *data)
{
	Event *event;

	event = clarityHeapAllocateWithDestructor(heap, sizeof(Event),
		(ClarityHeapDestructor)eventDestroy);

	if (event) {
		event->data = clarityHeapRetain(data);
		event->function = function;
	}
	return clarityHeapAutoRelease(event);
}

static Bool hasEvent(ClarityEventLoop *eventLoop)
{
	Bool retVal = 0;
	if (eventLoop)
		retVal = clarityArrayLength(eventLoop->events);
	return retVal;
}

static void eventLoopDestroy(ClarityEventLoop *eventLoop)
{
	if (eventLoop)
		clarityHeapRelease(eventLoop->events);
}

static void dequeue(ClarityEventLoop *eventLoop)
{
	Event *event;

	if (eventLoop) {
		event = clarityArrayPop(eventLoop->events);
		if (event)
			event->function(event->data);
		clarityHeapCollectGarbage(clarityHeap(eventLoop));
	}
}

typedef ClarityArray *(*Adder)(ClarityArray *, void *);

static void clarityEventLoopAdd(ClarityEventLoop *eventLoop,
	ClarityEvent function, void *data, Adder adder)
{
	if (eventLoop) {
		Event *event;

		event = eventCreate(clarityHeap(eventLoop), function, data);
		adder(eventLoop->events, event);
	}
}

void clarityEventLoopEnqueue(ClarityEventLoop *eventLoop,
	ClarityEvent function, void *data)
{
	clarityEventLoopAdd(eventLoop, function, data, clarityArrayUnshift);
}

void clarityEventLoopStart(ClarityEventLoop *eventLoop)
{
	while (hasEvent(eventLoop))
		dequeue(eventLoop);
}

ClarityEventLoop *clarityEventLoopCreate(ClarityHeap *heap)
{
	ClarityEventLoop *eventLoop;

	eventLoop = clarityHeapAllocateWithDestructor(heap,
		sizeof(ClarityEventLoop),
		(ClarityHeapDestructor)eventLoopDestroy);

	if (eventLoop)
		eventLoop->events = clarityHeapRetain(clarityArrayCreate(heap));

	return clarityHeapAutoRelease(eventLoop);
}
