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
	ClarityEvent function;
	void *data;
} Event;

struct __ClarityEventLoop {
	ClarityArray *events;
};

static void eventDestroy(Event *event)
{
	clarityRelease(event->data);
}

static Event *eventCreate(ClarityCore *core, ClarityEvent function,
	void *data)
{
	Event *event;

	event = clarityAllocateWithDestructor(core, sizeof(Event),
		(ClarityDestructor)eventDestroy);

	event->data = clarityRetain(data);
	event->function = function;
	return clarityAutoRelease(event);
}

static Bool hasEvent(ClarityEventLoop *eventLoop)
{
	return clarityArrayLength(eventLoop->events);
}

static void eventLoopDestroy(ClarityEventLoop *eventLoop)
{
	clarityRelease(eventLoop->events);
}

static void dequeue(ClarityEventLoop *eventLoop)
{
	Event *event;

	event = clarityArrayPop(eventLoop->events);
	event->function(event->data);
	clarityCollectGarbage(clarityCore());
}

typedef ClarityArray *(*Adder)(ClarityArray *, void *);

static void clarityEventLoopAdd(ClarityEventLoop *eventLoop,
	ClarityEvent function, void *data, Adder adder)
{
	Event *event;

	event = eventCreate(clarityCore(), function, data);
	adder(eventLoop->events, event);
}

void clarityEventLoopEnqueue(ClarityEventLoop *eventLoop,
	ClarityEvent function, void *data)
{
	clarityEventLoopAdd(eventLoop, function, data, clarityArrayUnshift);
}

void clarityEventLoopPush(ClarityEventLoop *eventLoop, ClarityEvent function,
	void *data)
{
	clarityEventLoopAdd(eventLoop, function, data, clarityArrayPush);
}

void clarityEventLoopStart(ClarityEventLoop *eventLoop)
{
	while (hasEvent(eventLoop))
		dequeue(eventLoop);
}

ClarityEventLoop *clarityEventLoopCreate(ClarityCore *core,
	ClarityEvent entry, void *data)
{
	ClarityEventLoop *eventLoop;

	eventLoop = clarityAllocateWithDestructor(core, sizeof(ClarityEventLoop),
		(ClarityDestructor)eventLoopDestroy);

	eventLoop->events = clarityRetain(clarityArrayCreate(core));
	clarityEventLoopEnqueue(eventLoop, entry, data);
	return clarityAutoRelease(eventLoop);
}
