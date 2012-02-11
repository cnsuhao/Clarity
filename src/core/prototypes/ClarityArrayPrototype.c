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
#include "ClarityObject.h"
#include "ClarityIntegerObject.h"
#include "ClarityBooleanObject.h"
#include "ClarityFunctionObject.h"

static ClarityEventLoop *gEventLoop = NULL;
static ClarityObject *gUndefined = NULL;

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
	clarityHeapRelease(iterator->arrayIterator);
	clarityHeapRelease(iterator->scope);
	clarityHeapRelease(iterator->handler);
	clarityHeapRelease(iterator->array);
}

static void iteratorNext(Iterator *iterator)
{
	iterator->data = clarityArrayIteratorNextItem(iterator->arrayIterator);
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

static void iteratorStart(Iterator *iterator)
{
	clarityEventLoopEnqueue(gEventLoop, (ClarityEvent)iterator->next,
		iterator);
}

static Iterator *iteratorCreate(ClarityHeap *heap, ClarityArray *array,
	void *scope, ClarityEvent handle, ClarityEvent done, void *handler)
{
	Iterator *iterator;

	iterator = clarityHeapAllocateWithDestructor(heap, sizeof(Iterator),
		(ClarityHeapDestructor)iteratorDestroy);

	iterator->arrayIterator = clarityHeapRetain(
			clarityArrayIteratorCreate(clarityHeap(heap), array));
	iterator->scope = clarityHeapRetain(scope);
	iterator->handle = handle;
	iterator->data = NULL;
	iterator->index = -1;
	iterator->array = clarityHeapRetain(array);
	iterator->done = done;
	iterator->next = (ClarityEvent)iteratorNext;
	iterator->handler = clarityHeapRetain(handler);
	return clarityHeapAutoRelease(iterator);
}

static Test *testCreate(ClarityHeap *heap)
{
	Test *test = clarityHeapAllocate(heap, sizeof(Test));

	test->retVal = TRUE;
	return clarityHeapAutoRelease(test);
}

static Bool testFunction(Iterator *iterator)
{
	ClarityObject *scope = clarityObjectCreate(clarityHeap(iterator));

	clarityObjectSetMember(scope, "prototype", iterator->scope);
	clarityObjectSetMember(scope, "$1", iterator->data);

	clarityObjectSetMember(scope, "$2",
		clarityIntegerObjectCreate(clarityHeap(iterator),
		iterator->index));

	clarityObjectSetMember(scope, "$3",
		clarityObjectGetMember(iterator->scope, "this"));

	return clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(clarityObjectGetMember(
		iterator->scope, "$1"), scope));
}

static void testCallback(Iterator *iterator)
{
	Test *test = (Test *)iterator->handler;
	ClarityObject *scope = clarityObjectCreate(clarityHeap(iterator));

	clarityObjectSetMember(scope, "$1",
		clarityBooleanObjectCreate(clarityHeap(iterator),
		test->retVal));

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(iterator->scope, "$2"), scope);
}

static void everyHandler(Iterator *iterator)
{
	Test *test = (Test *)iterator->handler;

	if (test->retVal)
		test->retVal = testFunction(iterator);
}

static void someHandler(Iterator *iterator)
{
	Test *test;
	Bool retVal;

	test = (Test *)iterator->handler;
	retVal = testFunction(iterator);
	test->retVal = test->retVal || retVal;
}

static void filterHandler(Iterator *iterator)
{
	ClarityObject *scope = clarityObjectCreate(clarityHeap(iterator));

	clarityObjectSetMember(scope, "prototype", iterator->scope);
	clarityObjectSetMember(scope, "$1", iterator->data);

	clarityObjectSetMember(scope, "$2",
		clarityIntegerObjectCreate(clarityHeap(iterator),
		iterator->index));

	clarityObjectSetMember(scope, "$3",
		clarityObjectGetMember(iterator->scope, "this"));

	if (clarityBooleanObjectGetValue(
		clarityFunctionObjectCall(clarityObjectGetMember(
		iterator->scope, "$1"), scope))) {
		clarityArrayPush(iterator->handler, iterator->data);
	}
}

static ClarityObject *length(ClarityObject *scope)
{
	ClarityObject *retVal = gUndefined;

	if (scope) {
		ClarityHeap *heap = clarityHeap(scope);

		if (clarityStrCmp(clarityObjectTypeOf(
		clarityObjectGetMember(scope, "this")), "array") == 0) {
			Uint32 length = clarityArrayLength(
				clarityObjectGetInnerData(
				clarityObjectGetMember(scope,
				"this")));

			retVal = clarityIntegerObjectCreate(heap, length);
		}
	}
	return retVal;
}

static void forEachFunction(Iterator *iterator)
{
	ClarityObject *scope = clarityObjectCreate(clarityHeap(iterator));

	clarityObjectSetMember(scope, "prototype", iterator->scope);
	clarityObjectSetMember(scope, "$1", iterator->data);

	clarityObjectSetMember(scope, "$2",
		clarityIntegerObjectCreate(clarityHeap(iterator),
		iterator->index));

	clarityObjectSetMember(scope, "$3",
		clarityObjectGetMember(iterator->scope, "this"));

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(iterator->scope, "$1"), scope);
}

static void mapHandler(Iterator *iterator)
{
	ClarityArray *newArray = iterator->handler;
	void *newItem;

	ClarityObject *scope = clarityObjectCreate(clarityHeap(iterator));

	clarityObjectSetMember(scope, "prototype", iterator->scope);
	clarityObjectSetMember(scope, "$1", iterator->data);

	clarityObjectSetMember(scope, "$2",
		clarityIntegerObjectCreate(clarityHeap(iterator),
		iterator->index));

	clarityObjectSetMember(scope, "$3",
		clarityObjectGetMember(iterator->scope, "this"));

	newItem = clarityFunctionObjectCall(
		clarityObjectGetOwnMember(iterator->scope, "$1"), scope);
	clarityArrayPush(newArray, newItem);
}

static void forEachCallback(Iterator *iterator)
{
	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(iterator->scope, "$2"),
		iterator->scope);
}

static void mapDone(Iterator *iterator)
{

	ClarityObject *scope = clarityObjectCreate(clarityHeap(iterator));

	clarityObjectSetMember(scope, "$1",
		clarityArrayObjectCreate(clarityHeap(iterator),
		(ClarityArray *)iterator->handler));

	clarityFunctionObjectCall(
		clarityObjectGetOwnMember(iterator->scope, "$2"), scope);
}

static ClarityObject *forEach(ClarityObject *scope)
{
	Iterator *iterator;

	iterator = iteratorCreate(clarityHeap(scope),
		clarityObjectGetInnerData(
		clarityObjectGetMember(scope, "this")),
		scope, (ClarityEvent)forEachFunction,
		(ClarityEvent)forEachCallback, NULL);

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
		clarityArrayCreate(clarityHeap(scope)));

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

	test->retVal = FALSE;
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
		clarityArrayCreate(clarityHeap(scope)));

	iteratorStart(iterator);
	return gUndefined;
}

ClarityObject *clarityArrayPrototypeCreate(ClarityHeap *heap)
{
	ClarityObject *prototype = clarityObjectCreate(heap);

	clarityObjectSetMember(prototype, "length",
			clarityFunctionObjectCreate(heap,
				length, gUndefined));

		clarityObjectSetMember(prototype, "forEach",
			clarityFunctionObjectCreate(heap,
				forEach, gUndefined));

		clarityObjectSetMember(prototype, "map",
				clarityFunctionObjectCreate(heap,
					map, gUndefined));

		clarityObjectSetMember(prototype, "every",
				clarityFunctionObjectCreate(heap,
					every, gUndefined));

		clarityObjectSetMember(prototype, "some",
				clarityFunctionObjectCreate(heap,
					some, gUndefined));

		clarityObjectSetMember(prototype, "filter",
				clarityFunctionObjectCreate(heap,
					filter, gUndefined));

		clarityObjectLock(prototype);
		return prototype;
}

