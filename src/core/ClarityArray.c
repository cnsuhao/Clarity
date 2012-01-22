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

typedef struct __Element Element;
struct __Element {
	void *data;
	Element *next;
	Element *prev;
};

struct __ClarityArray {
	Element *last;
	Element *first;
	Uint32 length;
	Bool locked;
};

typedef struct {
	Sint32 index;
	Element *element;
	ClarityArray *array;
	void *scope ;
	ClarityEvent handle;
	ClarityEvent done;
	void *handler;
} Iterator;

typedef struct {
	Bool retVal;
	ClarityArrayTestFunction function;
	ClarityArrayTestCallback callback;
} Test;

typedef struct {
	ClarityArrayTestFunction function;
	ClarityArrayMapCallback callback;
	void *newArray;
} Filter;

typedef struct {
	ClarityArrayForEachFunction function;
	ClarityArrayForEachCallback callback;
} ForEach;

typedef struct {
	Iterator *iterator;
	ClarityArrayMapFunction function;
	ClarityArrayMapCallback callback;
	void *newArray;
} Map;

static void iteratorDestroy(Iterator *iterator)
{
	clarityRelease(iterator->element);
	clarityRelease(iterator->scope);
	clarityRelease(iterator->array);
	clarityRelease(iterator->handler);
}

static Bool iteratorHasItem(Iterator *iterator)
{
	return iterator->element != iterator->array->last;
}

static void iteratorEvent(Iterator *iterator)
{
	Element *element = iterator->element->next;

	clarityRelease(iterator->element);
	iterator->element = clarityRetain(element);
	iterator->index++;

	if (iteratorHasItem(iterator)) {
		clarityEnqueueEvent(clarityCore(),
			iterator->handle, iterator);

		clarityEnqueueEvent(clarityCore(),
			(ClarityEvent)iteratorEvent, iterator);
	} else
		clarityEnqueueEvent(clarityCore(),
			iterator->done, iterator);
}

static void iteratorStart(Iterator *iterator)
{
	clarityEnqueueEvent(clarityCore(), (ClarityEvent)iteratorEvent,
		iterator);
}

static Iterator *iteratorCreate(ClarityCore *core, ClarityArray *array,
	void *scope, ClarityEvent handle, ClarityEvent done, void *handler)
{
	Iterator *iterator;

	iterator = clarityAllocateWithDestructor(core, sizeof(Iterator),
		(ClarityDestructor)iteratorDestroy);

	iterator->index = -1;
	iterator->array = clarityRetain(array);
	iterator->element = clarityRetain(array->first);
	iterator->scope = clarityRetain(scope);
	iterator->handle = handle;
	iterator->done = done;
	iterator->handler = clarityRetain(handler);
	return clarityAutoRelease(iterator);
}

static Test *testCreate(ClarityCore *core, ClarityArrayTestFunction function,
	ClarityArrayTestCallback callback)
{
	Test *test;

	test = clarityAllocate(core, sizeof(Test));
	test->function = function;
	test->callback = callback;
	return clarityAutoRelease(test);
}

static void everyHandler(Iterator *iterator)
{
	Test *test;

	test = (Test *)iterator->handler;

	if (test->retVal) {
		test->retVal = test->function(iterator->element->data,
			iterator->index, iterator->array, iterator->scope);
	}
}

static void everyDone(Iterator *iterator)
{
	Test *test;

	test = (Test *)iterator->handler;
	test->callback(test->retVal, iterator->scope);
}

void clarityArrayEvery(ClarityArray *array, ClarityArrayTestFunction function,
	ClarityArrayTestCallback callback, void *scope)
{
	Test *test;
	Iterator *iterator;

	test = testCreate(clarityCore(), function, callback);
	iterator = iteratorCreate(clarityCore(), array, scope,
		(ClarityEvent)everyHandler, (ClarityEvent)everyDone, test);

	test->retVal = TRUE;
	iteratorStart(iterator);
}

static void someHandler(Iterator *iterator)
{
	Test *test;
	Bool retVal;

	test = (Test *)iterator->handler;
	retVal = test->function(iterator->element->data, iterator->index,
		iterator->array, iterator->scope);

	test->retVal = test->retVal || retVal;
}

static void someDone(Iterator *iterator)
{
	Test *test;

	test = (Test *)iterator->handler;
	test->callback(test->retVal, iterator->scope);
}

void clarityArraySome(ClarityArray *array, ClarityArrayTestFunction function,
	ClarityArrayTestCallback callback, void *scope)
{
	Test *test;
	Iterator *iterator;

	test = testCreate(clarityCore(), function, callback);

	iterator = iteratorCreate(clarityCore(), array, scope,
		(ClarityEvent)someHandler, (ClarityEvent)someDone, test);

	test->retVal = FALSE;
	iteratorStart(iterator);
}

static ForEach *forEachCreate(ClarityCore *core,
	ClarityArrayForEachFunction function,
	ClarityArrayForEachCallback callback)
{
	ForEach *forEach;

	forEach = clarityAllocate(core, sizeof(ForEach));

	forEach->function = function;
	forEach->callback = callback;
	return clarityAutoRelease(forEach);
}

static void forEachHandler(Iterator *iterator)
{
	ForEach *forEach;

	forEach = (ForEach *)iterator->handler;
	forEach->function(iterator->element->data, iterator->index,
		iterator->array, iterator->scope);
}

static void forEachDone(Iterator *iterator)
{
	ForEach *forEach;

	forEach = (ForEach *)iterator->handler;
	forEach->callback(iterator->scope);
}

static void emptyForEachCallback(void *data)
{
}

void clarityArrayForEachWithoutCallback(
	ClarityArray *array,
	ClarityArrayForEachFunction function)
{
	clarityArrayForEach(array, function, emptyForEachCallback, NULL);
}

void clarityArrayForEach(ClarityArray *array,
	ClarityArrayForEachFunction function, ClarityArrayForEachCallback callback,
	void *scope)
{
	ForEach *forEach;
	Iterator *iterator;

	forEach = forEachCreate(clarityCore(), function, callback);
	iterator = iteratorCreate(clarityCore(), array, scope,
		(ClarityEvent)forEachHandler, (ClarityEvent)forEachDone, forEach);

	iteratorStart(iterator);
}

static void mapDestroy(Map *map)
{
	clarityRelease(map->newArray);
}

static Map *mapCreate(ClarityCore *core, ClarityArrayMapFunction function,
	ClarityArrayMapCallback callback)
{
	Map *map;

	map = clarityAllocateWithDestructor(core, sizeof(Map),
		(ClarityDestructor)mapDestroy);

	map->newArray = clarityRetain(clarityArrayCreate(core));
	map->function = function;
	map->callback = callback;
	return clarityAutoRelease(map);
}

static void mapHandler(Iterator *iterator)
{
	Map *map;
	void *newItem;

	map = (Map *)iterator->handler;
	newItem = map->function(iterator->element->data, iterator->index,
		iterator->array, iterator->scope);

	clarityArrayPush(map->newArray, newItem);
}

static void mapDone(Iterator *iterator)
{
	Map *map;

	map = (Map *)iterator->handler;
	map->callback(map->newArray, iterator->scope);
}

void clarityArrayMap(ClarityArray *array, ClarityArrayMapFunction function,
	ClarityArrayMapCallback callback, void *scope)
{
	Map *map;
	Iterator *iterator;

	map = mapCreate(clarityCore(), function, callback);

	iterator = iteratorCreate(clarityCore(), array, scope,
		(ClarityEvent)mapHandler, (ClarityEvent)mapDone, map);

	iteratorStart(iterator);
}

static void filterDestroy(Filter *filter)
{
	clarityRelease(filter->newArray);
}

static Filter *filterCreate(ClarityCore *core,
	ClarityArrayTestFunction function, ClarityArrayMapCallback callback)
{
	Filter *filter;

	filter = clarityAllocateWithDestructor(core, sizeof(Filter),
		(ClarityDestructor)filterDestroy);

	filter->newArray = clarityRetain(clarityArrayCreate(core));
	filter->function = function;
	filter->callback = callback;
	return clarityAutoRelease(filter);
}

static void filterHandler(Iterator *iterator)
{
	Filter *filter;
	Bool match;

	filter = (Filter *)iterator->handler;

	match = filter->function(iterator->element->data,
		iterator->index, iterator->array, iterator->scope);

	if (match)
		clarityArrayPush(filter->newArray, iterator->element->data);
}

static void filterDone(Iterator *iterator)
{
	Filter *filter;

	filter = (Filter *)iterator->handler;
	filter->callback(filter->newArray, iterator->scope);
}

void clarityArrayFilter(ClarityArray *array, ClarityArrayTestFunction function,
	ClarityArrayMapCallback callback, void *scope)
{
	Filter *filter;
	Iterator *iterator;

	filter = filterCreate(clarityCore(), function, callback);

	iterator = iteratorCreate(clarityCore(),
		array, scope, (ClarityEvent)filterHandler,
		(ClarityEvent)filterDone, filter);

	iteratorStart(iterator);
}

static void elementDestroy(Element *element)
{
	clarityRelease(element->data);
}

static Element *elementCreate(ClarityCore *core, void *data)
{
	Element *element;

	element = clarityAllocateWithDestructor(core,
		sizeof(Element), (ClarityDestructor)elementDestroy);

	element->data = clarityRetain(data);
	element->next = NULL;
	element->prev = NULL;
	return clarityAutoRelease(element);
}

static void arrayDestroy(ClarityArray *array)
{
	Element *item;
	Element *next;

	item = array->first->next;

	while (item != array->last) {
		next = item->next;
		clarityRelease(item);
		item = next;
	}
	clarityRelease(array->last);
	clarityRelease(array->first);
}

ClarityArray *clarityArrayUnshift(ClarityArray *array, void *data)
{
	if (array && !array->locked) {
		Element *newElement;

		newElement = elementCreate(clarityCore(), data);
		newElement = clarityRetain(newElement);
		newElement->next = array->first->next;
		newElement->prev = array->first;
		newElement->next->prev = newElement;
		array->first->next = newElement;
		array->length++;
	}
	return array;
}

void *clarityArrayShift(ClarityArray *array)
{
	void *retVal;

	retVal = NULL;

	if (clarityArrayLength(array) && !array->locked) {
		Element *element;

		element = array->first->next;
		array->first->next = element->next;
		array->first->next->prev = array->first;
		array->length--;
		retVal = clarityRetain(element->data);
		clarityRelease(element);
		clarityAutoRelease(retVal);
	}
	return retVal;

}

ClarityArray *clarityArrayPush(ClarityArray *array, void *data)
{
	if (array && !array->locked) {
		Element *newElement;

		newElement = elementCreate(clarityCore(), data);
		newElement = clarityRetain(newElement);
		newElement->prev = array->last->prev;
		newElement->next = array->last;
		array->last->prev->next = newElement;
		array->last->prev = newElement;
		array->length++;
	}
	return array;
}

void *clarityArrayPop(ClarityArray *array)
{
	void *retVal;

	retVal = NULL;

	if (clarityArrayLength(array) && !array->locked) {
		Element *element;

		element = array->last->prev;
		array->last->prev = element->prev;
		array->last->prev->next = array->last;
		array->length--;
		retVal = clarityRetain(element->data);
		clarityRelease(element);
		clarityAutoRelease(retVal);
	}
	return retVal;
}

ClarityArray *clarityArrayLock(ClarityArray *array)
{
	if (array)
		array->locked = TRUE;
	return array;
}

Uint32 clarityArrayLength(ClarityArray *array)
{
	return array->length;
}

ClarityArray *clarityArrayCreate(ClarityCore *core)
{
	ClarityArray *array;

	array = clarityAllocateWithDestructor(core,
		sizeof(ClarityArray), (ClarityDestructor)arrayDestroy);

	array->length = 0;
	array->locked = FALSE;
	array->first = clarityRetain(elementCreate(core, NULL));
	array->last = clarityRetain(elementCreate(core, NULL));
	array->first->next = array->last;
	array->last->prev = array->first;
	return clarityAutoRelease(array);
}
