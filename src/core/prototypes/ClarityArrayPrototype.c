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
#include "ClarityArray.h"
#include "ClarityArrayObject.h"
#include "ClarityObjectPriv.h"
#include "ClarityEventLoop.h"
#include "ClarityNumberObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityFunctionObject.h"

static ClarityEventLoop *gEventLoop = 0;
static ClarityObject *gUndefined = 0;

void clarityArrayPrototypeStaticInitializer(
	ClarityEventLoop *eventLoop, ClarityObject *undefined)
{
	gEventLoop = clarityHeapRetain(eventLoop);
	gUndefined = clarityHeapRetain(undefined);
}

void clarityArrayPrototypeStaticRelease(void)
{
	clarityHeapRelease(gUndefined);
	clarityHeapRelease(gEventLoop);
}

typedef struct {
	ClarityArrayIterator *arrayIterator;
	Uint32 index;
	void *data;
	ClarityArray *array;
	void *scope;
	ClarityEvent handle;
	ClarityEvent done;
	ClarityEvent next;
	void *handler;
} Iterator;

typedef struct {
	Bool retVal;
} Test;

static void iteratorDestroy(Iterator *iterator)
{
	if (iterator) {
		clarityHeapRelease(iterator->arrayIterator);
		clarityHeapRelease(iterator->scope);
		clarityHeapRelease(iterator->handler);
		clarityHeapRelease(iterator->array);
	}
}

static void iteratorNext(Iterator *iterator)
{
	if (iterator) {
		iterator->data =
			clarityArrayIteratorNextItem(iterator->arrayIterator);

		iterator->index++;

		if (clarityArrayIteratorHasItem(iterator->arrayIterator)) {
			clarityEventLoopEnqueue(gEventLoop,
				iterator->handle, iterator);

			clarityEventLoopEnqueue(gEventLoop,
				(ClarityEvent)iterator->next, iterator);
		} else
			clarityEventLoopEnqueue(gEventLoop,
				iterator->done, iterator);
	}
}

static void iteratorStart(Iterator *iterator)
{
	if (iterator)
		clarityEventLoopEnqueue(gEventLoop,
			(ClarityEvent)iterator->next, iterator);
}

static Iterator *iteratorCreate(ClarityHeap *heap, ClarityArray *array,
	void *scope, ClarityEvent handle, ClarityEvent done, void *handler)
{
	Iterator *iterator;

	iterator = clarityHeapAllocateWithDestructor(heap, sizeof(Iterator),
		(ClarityHeapDestructor)iteratorDestroy);

	if (iterator) {
		iterator->arrayIterator = clarityHeapRetain(
			clarityArrayIteratorCreate(heap, array));
		iterator->scope = clarityHeapRetain(scope);
		iterator->handle = handle;
		iterator->data = 0;
		iterator->index = -1;
		iterator->array = clarityHeapRetain(array);
		iterator->done = done;
		iterator->next = (ClarityEvent)iteratorNext;
		iterator->handler = clarityHeapRetain(handler);
	}
	return clarityHeapAutoRelease(iterator);
}

static Test *testCreate(ClarityHeap *heap)
{
	Test *test = clarityHeapAllocate(heap, sizeof(Test));

	if (test)
		test->retVal = 1;

	return clarityHeapAutoRelease(test);
}

static Bool testFunction(Iterator *iterator)
{
	Bool retVal = 0;
	if (iterator) {
		ClarityObject *scope =
			clarityObjectCreate(clarityHeap(iterator));

		clarityObjectSetOwnMember(scope, "prototype", iterator->scope);
		clarityObjectSetOwnMember(scope, "$1", iterator->data);

		clarityObjectSetOwnMember(scope, "$2",
			clarityNumberObjectCreate(clarityHeap(iterator),
			iterator->index));

		clarityObjectSetOwnMember(scope, "$3",
			clarityObjectGetMember(iterator->scope, "this"));

		retVal = clarityBooleanObjectGetValue(
			clarityFunctionObjectCall(clarityObjectGetMember(
			iterator->scope, "$1"), scope));
	}
	return retVal;
}

static void testCallback(Iterator *iterator)
{
	if (iterator) {
		Test *test = (Test *)iterator->handler;
		ClarityObject *scope =
			clarityObjectCreate(clarityHeap(iterator));

		clarityObjectSetOwnMember(scope, "$1",
			clarityBooleanObjectCreate(clarityHeap(iterator),
			test->retVal));

		clarityFunctionObjectCall(
			clarityObjectGetOwnMember(iterator->scope, "$2"),
			scope);
	}
}

static void everyHandler(Iterator *iterator)
{
	if (iterator) {
		Test *test = (Test *)iterator->handler;

		if (test && test->retVal)
			test->retVal = testFunction(iterator);
	}
}

static void someHandler(Iterator *iterator)
{
	if (iterator) {
		Test *test;
		Bool retVal;

		test = (Test *)iterator->handler;
		if (test) {
			retVal = testFunction(iterator);
			test->retVal = test->retVal || retVal;
		}
	}
}

static void filterHandler(Iterator *iterator)
{
	if (iterator) {
		ClarityObject *scope =
			clarityObjectCreate(clarityHeap(iterator));

		clarityObjectSetOwnMember(scope, "prototype", iterator->scope);
		clarityObjectSetOwnMember(scope, "$1", iterator->data);

		clarityObjectSetOwnMember(scope, "$2",
			clarityNumberObjectCreate(clarityHeap(iterator),
			iterator->index));

		clarityObjectSetOwnMember(scope, "$3",
			clarityObjectGetMember(iterator->scope, "this"));

		if (clarityBooleanObjectGetValue(
			clarityFunctionObjectCall(clarityObjectGetMember(
			iterator->scope, "$1"), scope))) {
			clarityArrayObjectPush(iterator->handler,
				iterator->data);
		}
	}
}

static ClarityObject *length(ClarityObject *scope)
{
	ClarityObject *retVal = gUndefined;

	ClarityHeap *heap = clarityHeap(scope);

	if (clarityObjectIsTypeOf(
		clarityObjectGetMember(scope, "this"), "array")) {
		Uint32 length = clarityArrayLength(
			clarityObjectGetInnerData(
			clarityObjectGetMember(scope,
			"this")));

		retVal = clarityNumberObjectCreate(heap, length);
	}
	return retVal;
}

static void forEachFunction(Iterator *iterator)
{
	if (iterator) {
		ClarityObject *scope =
			clarityObjectCreate(clarityHeap(iterator));

		clarityObjectSetOwnMember(scope, "prototype", iterator->scope);
		clarityObjectSetOwnMember(scope, "$1", iterator->data);

		clarityObjectSetOwnMember(scope, "$2",
			clarityNumberObjectCreate(clarityHeap(iterator),
			iterator->index));

		clarityObjectSetOwnMember(scope, "$3",
			clarityObjectGetMember(iterator->scope, "this"));

		clarityFunctionObjectCall(
			clarityObjectGetOwnMember(iterator->scope, "$1"),
			scope);
	}
}

static void mapHandler(Iterator *iterator)
{
	if (iterator) {
		ClarityObject *newArray = iterator->handler;
		void *newItem;

		ClarityObject *scope =
			clarityObjectCreate(clarityHeap(iterator));

		clarityObjectSetOwnMember(scope, "prototype", iterator->scope);
		clarityObjectSetOwnMember(scope, "$1", iterator->data);

		clarityObjectSetOwnMember(scope, "$2",
			clarityNumberObjectCreate(clarityHeap(iterator),
			iterator->index));

		clarityObjectSetOwnMember(scope, "$3",
			clarityObjectGetMember(iterator->scope, "this"));

		newItem = clarityFunctionObjectCall(
			clarityObjectGetOwnMember(iterator->scope, "$1"),
			scope);

		clarityArrayObjectPush(newArray, newItem);
	}
}

static void forEachCallback(Iterator *iterator)
{
	if (iterator)
		clarityFunctionObjectCall(
			clarityObjectGetOwnMember(iterator->scope, "$2"),
			iterator->scope);
}

static void mapDone(Iterator *iterator)
{
	if (iterator) {
		ClarityObject *scope =
			clarityObjectCreate(clarityHeap(iterator));

		clarityObjectSetOwnMember(scope, "$1", iterator->handler);

		clarityFunctionObjectCall(
			clarityObjectGetOwnMember(iterator->scope, "$2"),
			scope);
	}
}

static ClarityObject *forEach(ClarityObject *scope)
{
	Iterator *iterator;

	iterator = iteratorCreate(clarityHeap(scope),
		clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		scope, (ClarityEvent)forEachFunction,
		(ClarityEvent)forEachCallback, 0);

	iteratorStart(iterator);
	return gUndefined;
}

static ClarityObject *map(ClarityObject *scope)
{
	Iterator *iterator;

	iterator = iteratorCreate(clarityHeap(scope),
		clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		scope, (ClarityEvent)mapHandler, (ClarityEvent)mapDone,
		clarityArrayObjectCreate(clarityHeap(scope)));

	iteratorStart(iterator);
	return gUndefined;
}

static  ClarityObject *every(ClarityObject *scope)
{
	Iterator *iterator;

	iterator = iteratorCreate(clarityHeap(scope),
		clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		scope, (ClarityEvent)everyHandler, (ClarityEvent)testCallback,
		testCreate(clarityHeap(scope)));

	iteratorStart(iterator);
	return gUndefined;
}

static ClarityObject *some(ClarityObject *scope)
{
	Test *test;
	Iterator *iterator;

	test = testCreate(clarityHeap(scope));

	iterator = iteratorCreate(clarityHeap(scope),
		clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		scope, (ClarityEvent)someHandler,
		(ClarityEvent)testCallback, test);

	test->retVal = 0;
	iteratorStart(iterator);
	return gUndefined;
}

static  ClarityObject *filter(ClarityObject *scope)
{
	Iterator *iterator;

	iterator = iteratorCreate(clarityHeap(scope),
		clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		scope, (ClarityEvent)filterHandler,
		(ClarityEvent)mapDone,
		clarityArrayObjectCreate(clarityHeap(scope)));

	iteratorStart(iterator);
	return gUndefined;
}

ClarityObject *clarityArrayPrototypeCreate(ClarityHeap *heap)
{
	ClarityObject *prototype = clarityObjectCreate(heap);

	clarityObjectSetOwnMember(prototype, "length",
			clarityFunctionObjectCreate(heap,
				length, gUndefined));

	clarityObjectSetOwnMember(prototype, "forEach",
		clarityFunctionObjectCreate(heap,
		forEach, gUndefined));

	clarityObjectSetOwnMember(prototype, "map",
		clarityFunctionObjectCreate(heap,
		map, gUndefined));

	clarityObjectSetOwnMember(prototype, "every",
		clarityFunctionObjectCreate(heap,
		every, gUndefined));

	clarityObjectSetOwnMember(prototype, "some",
		clarityFunctionObjectCreate(heap,
		some, gUndefined));

	clarityObjectSetOwnMember(prototype, "filter",
		clarityFunctionObjectCreate(heap,
		filter, gUndefined));

	return clarityObjectLock(prototype);
}

