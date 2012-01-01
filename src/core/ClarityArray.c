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
#include "ClarityHeap.h"

typedef struct __Element Element;
struct __Element {
	void *data;
	Element *next;
	Element *prev;
};

struct __ClarityArray {
	Clarity *clarity;
	Element *last;
	Element *first;
	Uint32 length;
};

typedef struct {
	Sint32 index;
	Element *element;
	ClarityArray *array;
	Clarity *clarity;
	void *data;
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

static void iteratorDestroy(ClarityHeap *heap,
							Iterator *iterator)
{
	clarityHeapRelease(heap, iterator->element);
	clarityHeapRelease(heap, iterator->data);
	clarityHeapRelease(heap, iterator->clarity);
	clarityHeapRelease(heap, iterator->array);
	clarityHeapRelease(heap, iterator->handler);
}

static Bool iteratorHasItem(Iterator *iterator)
{
	return iterator->element != iterator->array->last;
}

static void iteratorEvent(Clarity *clarity, Iterator *iterator)
{
	ClarityHeap *heap;
	Element *element = iterator->element->next;

	heap = clarityGetHeap(iterator->clarity);
	clarityHeapRelease(heap, iterator->element);
	iterator->element = clarityHeapRetain(heap, element);
	iterator->index++;

	if (iteratorHasItem(iterator)) {
		clarityEnqueueEvent(iterator->clarity,
							iterator->handle,
							iterator);

		clarityEnqueueEvent(iterator->clarity,
							(ClarityEvent)iteratorEvent,
							iterator);
	} else
		clarityEnqueueEvent(iterator->clarity,
							iterator->done,
							iterator);
}

static void iteratorStart(Iterator *iterator)
{
	clarityEnqueueEvent(iterator->clarity,
						(ClarityEvent)iteratorEvent,
						iterator);
}

static Iterator *iteratorCreate(Clarity *clarity,
								ClarityArray *array,
								void *data,
								ClarityEvent handle,
								ClarityEvent done,
								void *handler)
{
	Iterator *iterator;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	iterator = clarityHeapAllocate(heap,
								   sizeof(Iterator),
								   (ClarityHeapDestructor)iteratorDestroy);

	iterator->index = -1;
	iterator->clarity = clarityHeapRetain(heap, clarity);
	iterator->array = clarityHeapRetain(heap, array);
	iterator->element = clarityHeapRetain(heap, array->first);
	iterator->data = clarityHeapRetain(heap, data);
	iterator->handle = handle;
	iterator->done = done;
	iterator->handler = clarityHeapRetain(heap, handler);
	clarityHeapAutoRelease(heap, iterator);
	return iterator;
}

static void emptyDestroy(ClarityHeap *heap, void *none)
{
}

static Test *testCreate(Clarity *clarity,
						ClarityArrayTestFunction function,
						ClarityArrayTestCallback callback)
{
	Test *test;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	test = clarityHeapAllocate(heap,
							   sizeof(Test),
							   (ClarityHeapDestructor)emptyDestroy);

	test->function = function;
	test->callback = callback;
	clarityHeapAutoRelease(heap, test);
	return test;
}
static void everyHandler(Clarity *clarity, Iterator *iterator)
{
	Test *test;

	test = (Test *)iterator->handler;

	if (test->retVal) {
		test->retVal = test->function(iterator->element->data,
									  iterator->index,
									  iterator->array,
									  clarity);
	}
}

static void everyDone(Clarity *clarity, Iterator *iterator)
{
	Test *test;

	test = (Test *)iterator->handler;
	test->callback(clarity, test->retVal, iterator->data);
}

void clarityArrayEvery(ClarityArray *array,
					   ClarityArrayTestFunction function,
					   ClarityArrayTestCallback callback,
					   void *data)
{
	Test *test;
	Iterator *iterator;

	test = testCreate(array->clarity,
					  function,
					  callback);

	iterator = iteratorCreate(array->clarity,
							  array,
							  data,
							  (ClarityEvent)everyHandler,
							  (ClarityEvent)everyDone,
							  test);

	test->retVal = TRUE;
	iteratorStart(iterator);
}

static void someHandler(Clarity *clarity, Iterator *iterator)
{
	Test *test;
	Bool retVal;

	test = (Test *)iterator->handler;
	retVal = test->function(iterator->element->data,
							iterator->index,
							iterator->array,
							clarity);

	test->retVal = test->retVal || retVal;
}

static void someDone(Clarity *clarity, Iterator *iterator)
{
	Test *test;

	test = (Test *)iterator->handler;
	test->callback(clarity, test->retVal, iterator->data);
}

void clarityArraySome(ClarityArray *array,
					  ClarityArrayTestFunction function,
					  ClarityArrayTestCallback callback,
					  void *data)
{
	Test *test;
	Iterator *iterator;

	test = testCreate(array->clarity,
					  function,
					  callback);

	iterator = iteratorCreate(array->clarity,
							  array,
							  data,
							  (ClarityEvent)someHandler,
							  (ClarityEvent)someDone,
							  test);

	test->retVal = FALSE;
	iteratorStart(iterator);
}

static ForEach *forEachCreate(Clarity *clarity,
							  ClarityArrayForEachFunction function,
							  ClarityArrayForEachCallback callback)
{
	ForEach *forEach;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	forEach = clarityHeapAllocate(heap,
								  sizeof(ForEach),
								  (ClarityHeapDestructor)emptyDestroy);

	forEach->function = function;
	forEach->callback = callback;
	clarityHeapAutoRelease(heap, forEach);
	return forEach;
}

static void forEachHandler(Clarity *clarity, Iterator *iterator)
{
	ForEach *forEach;

	forEach = (ForEach *)iterator->handler;
	forEach->function(iterator->element->data,
					  iterator->index,
					  iterator->array,
					  clarity);
}

static void forEachDone(Clarity *clarity, Iterator *iterator)
{
	ForEach *forEach;

	forEach = (ForEach *)iterator->handler;
	if (forEach->callback)
		forEach->callback(clarity, iterator->data);
}

void clarityArrayForEach(ClarityArray *array,
						 ClarityArrayForEachFunction function,
						 ClarityArrayForEachCallback callback,
						 void *data)
{
	ForEach *forEach;
	Iterator *iterator;

	forEach = forEachCreate(array->clarity,
							function,
							callback);

	iterator = iteratorCreate(array->clarity,
							  array,
							  data,
							  (ClarityEvent)forEachHandler,
							  (ClarityEvent)forEachDone,
							  forEach);

	iteratorStart(iterator);
}

static void mapDestroy(ClarityHeap *heap,
					   Map *map)
{
	clarityHeapRelease(heap, map->newArray);
}

static Map *mapCreate(Clarity *clarity,
					  ClarityArrayMapFunction function,
					  ClarityArrayMapCallback callback)
{
	Map *map;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	map = clarityHeapAllocate(heap,
							  sizeof(Map),
							  (ClarityHeapDestructor)mapDestroy);

	map->newArray = clarityArrayCreate(clarity);
	map->newArray = clarityHeapRetain(heap, map->newArray);
	map->function = function;
	map->callback = callback;
	clarityHeapAutoRelease(heap, map);
	return map;
}

static void mapHandler(Clarity *clarity, Iterator *iterator)
{
	Map *map;
	void *newItem;

	map = (Map *)iterator->handler;
	newItem = map->function(iterator->element->data,
							iterator->index,
							iterator->array,
							clarity);

	clarityArrayPush(map->newArray, newItem);
}

static void mapDone(Clarity *clarity, Iterator *iterator)
{
	Map *map;

	map = (Map *)iterator->handler;
	map->callback(clarity, map->newArray, iterator->data);
}

void clarityArrayMap(ClarityArray *array,
					 ClarityArrayMapFunction function,
					 ClarityArrayMapCallback callback,
					 void *data)
{
	Map *map;
	Iterator *iterator;

	map = mapCreate(array->clarity,
					function,
					callback);

	iterator = iteratorCreate(array->clarity,
							  array,
							  data,
							  (ClarityEvent)mapHandler,
							  (ClarityEvent)mapDone,
							  map);

	iteratorStart(iterator);
}

static void filterDestroy(ClarityHeap *heap,
						  Filter *filter)
{
	clarityHeapRelease(heap, filter->newArray);
}

static Filter *filterCreate(Clarity *clarity,
							ClarityArrayTestFunction function,
							ClarityArrayMapCallback callback)
{
	Filter *filter;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	filter = clarityHeapAllocate(heap,
								 sizeof(Filter),
								 (ClarityHeapDestructor)filterDestroy);

	filter->newArray = clarityArrayCreate(clarity);
	filter->newArray = clarityHeapRetain(heap, filter->newArray);
	filter->function = function;
	filter->callback = callback;
	clarityHeapAutoRelease(heap, filter);
	return filter;
}

static void filterHandler(Clarity *clarity, Iterator *iterator)
{
	Filter *filter;
	Bool match;

	filter = (Filter *)iterator->handler;

	match = filter->function(iterator->element->data,
							 iterator->index,
							 iterator->array,
							 clarity);

	if (match)
		clarityArrayPush(filter->newArray, iterator->element->data);
}

static void filterDone(Clarity *clarity, Iterator *iterator)
{
	Filter *filter;

	filter = (Filter *)iterator->handler;
	filter->callback(clarity, filter->newArray, iterator->data);
}

void clarityArrayFilter(ClarityArray *array,
						ClarityArrayTestFunction function,
						ClarityArrayMapCallback callback,
						void *data)
{
	Filter *filter;
	Iterator *iterator;

	filter = filterCreate(array->clarity,
						  function,
						  callback);

	iterator = iteratorCreate(array->clarity,
							  array,
							  data,
							  (ClarityEvent)filterHandler,
							  (ClarityEvent)filterDone,
							  filter);

	iteratorStart(iterator);
}

static void elementDestroy(ClarityHeap *heap, Element *element)
{
	clarityHeapRelease(heap, element->data);
}

static Element *elementCreate(Clarity *clarity, void *data)
{
	Element *element;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	element = clarityHeapAllocate(heap,
								  sizeof(Element),
								  (ClarityHeapDestructor)elementDestroy);

	element->data = clarityHeapRetain(heap, data);
	element->next = NULL;
	element->prev = NULL;
	clarityHeapAutoRelease(heap, element);
	return element;
}

static void destroy(ClarityHeap *heap, ClarityArray *array)
{
	Element *item;
	Element *next;

	clarityHeapRelease(heap, array->clarity);
	item = array->first->next;

	while (item != array->last) {
		next = item->next;
		clarityHeapRelease(heap, item);
		item = next;
	}
	clarityHeapRelease(heap, array->last);
	clarityHeapRelease(heap, array->first);
}

void clarityArrayUnshift(ClarityArray *array, void *data)
{
	ClarityHeap *heap;
	Element *newElement;

	heap = clarityGetHeap(array->clarity);
	newElement = elementCreate(array->clarity, data);
	newElement = clarityHeapRetain(heap, newElement);
	newElement->next = array->first->next;
	newElement->prev = array->first;
	newElement->next->prev = newElement;
	array->first->next = newElement;
	array->length++;
}

void *clarityArrayShift(ClarityArray *array)
{
	void *retVal;

	retVal = NULL;

	if (clarityArrayLength(array)) {
		ClarityHeap *heap;
		Element *element;

		element = array->first->next;
		array->first->next = element->next;
		array->first->next->prev = array->first;
		array->length--;
		heap = clarityGetHeap(array->clarity);
		retVal = clarityHeapRetain(heap, element->data);
		clarityHeapRelease(heap, element);
		clarityHeapAutoRelease(heap, retVal);
	}
	return retVal;

}

void clarityArrayPush(ClarityArray *array, void *data)
{
	ClarityHeap *heap;
	Element *newElement;

	heap = clarityGetHeap(array->clarity);
	newElement = elementCreate(array->clarity, data);
	newElement = clarityHeapRetain(heap, newElement);
	newElement->prev = array->last->prev;
	newElement->next = array->last;
	array->last->prev->next = newElement;
	array->last->prev = newElement;
	array->length++;
}

void *clarityArrayPop(ClarityArray *array)
{
	void *retVal;

	retVal = NULL;

	if (clarityArrayLength(array)) {
		ClarityHeap *heap;
		Element *element;

		element = array->last->prev;
		array->last->prev = element->prev;
		array->last->prev->next = array->last;
		array->length--;
		heap = clarityGetHeap(array->clarity);
		retVal = clarityHeapRetain(heap, element->data);
		clarityHeapRelease(heap, element);
		clarityHeapAutoRelease(heap, retVal);
	}
	return retVal;
}

Uint32 clarityArrayLength(ClarityArray *array)
{
	return array->length;
}

ClarityArray *clarityArrayCreate(Clarity *clarity)
{
	ClarityHeap *heap;
	ClarityArray *array;

	heap = clarityGetHeap(clarity);
	array = clarityHeapAllocate(heap,
								sizeof(ClarityArray),
								(ClarityHeapDestructor)destroy);

	array->clarity = clarityHeapRetain(heap, clarity);
	array->length = 0;
	array->first = clarityHeapRetain(heap, elementCreate(clarity, NULL));
	array->last = clarityHeapRetain(heap, elementCreate(clarity, NULL));
	array->first->next = array->last;
	array->last->prev = array->first;
	clarityHeapAutoRelease(heap, array);
	return array;
}
