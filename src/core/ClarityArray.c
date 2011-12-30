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
	Uint32 index;
	Bool retVal;
	Element *element;
	ClarityArray *array;
	Clarity *clarity;
	ClarityArrayTestFunction function;
	ClarityArrayTestCallback callback;
	void *data;
} Test;

typedef struct {
	Uint32 index;
	Element *element;
	ClarityArray *array;
	Clarity *clarity;
	ClarityArrayTestFunction function;
	ClarityArrayMapCallback callback;
	void *data;
	void *newArray;
} Filter;

typedef struct {
	Uint32 index;
	Element *element;
	ClarityArray *array;
	Clarity *clarity;
	ClarityArrayForEachFunction function;
	ClarityArrayForEachCallback callback;
	void *data;
} ForEach;

typedef struct {
	Uint32 index;
	Element *element;
	ClarityArray *array;
	Clarity *clarity;
	ClarityArrayMapFunction function;
	ClarityArrayMapCallback callback;
	void *data;
	void *newArray;
} Map;

static void testDestroy(ClarityHeap *heap,
						Test *test)
{
	clarityHeapRelease(heap, test->element);
	clarityHeapRelease(heap, test->data);
	clarityHeapRelease(heap, test->clarity);
	clarityHeapRelease(heap, test->array);
}

static void testSetElement(Test *test,
						   Element *element)
{
	ClarityHeap *heap;

	heap = clarityGetHeap(test->clarity);
	clarityHeapRelease(heap, test->element);
	test->element = clarityHeapRetain(heap, element);
	test->index++;
}

static Test *testCreate(Clarity *clarity,
						ClarityArray *array,
						Element *element,
						ClarityArrayTestFunction function,
						ClarityArrayTestCallback callback,
						void *data)
{
	Test *test;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	test = clarityHeapAllocate(heap,
							   sizeof(Test),
							   (ClarityHeapDestructor)testDestroy);

	test->index = 0;
	test->clarity = clarityHeapRetain(heap, clarity);
	test->array = clarityHeapRetain(heap, array);
	test->element = clarityHeapRetain(heap, element);
	test->data = clarityHeapRetain(heap, data);
	test->function = function;
	test->callback = callback;
	clarityHeapAutoRelease(heap, test);
	return test;
}
static void filterDestroy(ClarityHeap *heap,
						  Filter *filter)
{
	clarityHeapRelease(heap, filter->element);
	clarityHeapRelease(heap, filter->data);
	clarityHeapRelease(heap, filter->clarity);
	clarityHeapRelease(heap, filter->array);
	clarityHeapRelease(heap, filter->newArray);
}

static void filterSetElement(Filter *filter,
							 Element *element)
{
	ClarityHeap *heap;

	heap = clarityGetHeap(filter->clarity);
	clarityHeapRelease(heap, filter->element);
	filter->element = clarityHeapRetain(heap, element);
	filter->index++;
}

static Filter *filterCreate(Clarity *clarity,
							ClarityArray *array,
							Element *element,
							ClarityArrayTestFunction function,
							ClarityArrayMapCallback callback,
							void *data)
{
	Filter *filter;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	filter = clarityHeapAllocate(heap,
								 sizeof(Filter),
								 (ClarityHeapDestructor)filterDestroy);

	filter->index = 0;
	filter->newArray = clarityArrayCreate(clarity);
	filter->newArray = clarityHeapRetain(heap, filter->newArray);
	filter->clarity = clarityHeapRetain(heap, clarity);
	filter->array = clarityHeapRetain(heap, array);
	filter->element = clarityHeapRetain(heap, element);
	filter->data = clarityHeapRetain(heap, data);
	filter->function = function;
	filter->callback = callback;
	clarityHeapAutoRelease(heap, filter);
	return filter;
}

static void mapDestroy(ClarityHeap *heap,
						   Map *map)
{
	clarityHeapRelease(heap, map->element);
	clarityHeapRelease(heap, map->data);
	clarityHeapRelease(heap, map->clarity);
	clarityHeapRelease(heap, map->array);
	clarityHeapRelease(heap, map->newArray);
}

static void mapSetElement(Map *map,
						  Element *element)
{
	ClarityHeap *heap;

	heap = clarityGetHeap(map->clarity);
	clarityHeapRelease(heap, map->element);
	map->element = clarityHeapRetain(heap, element);
	map->index++;
}

static Map *mapCreate(Clarity *clarity,
					  ClarityArray *array,
					  Element *element,
					  ClarityArrayMapFunction function,
					  ClarityArrayMapCallback callback,
					  void *data)
{
	Map *map;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	map = clarityHeapAllocate(heap,
							  sizeof(Map),
							  (ClarityHeapDestructor)mapDestroy);

	map->index = 0;
	map->newArray = clarityArrayCreate(clarity);
	map->newArray = clarityHeapRetain(heap, map->newArray);
	map->clarity = clarityHeapRetain(heap, clarity);
	map->array = clarityHeapRetain(heap, array);
	map->element = clarityHeapRetain(heap, element);
	map->data = clarityHeapRetain(heap, data);
	map->function = function;
	map->callback = callback;
	clarityHeapAutoRelease(heap, map);
	return map;
}

static void forEachDestroy(ClarityHeap *heap,
						   ForEach *forEach)
{
	clarityHeapRelease(heap, forEach->element);
	clarityHeapRelease(heap, forEach->data);
	clarityHeapRelease(heap, forEach->clarity);
	clarityHeapRelease(heap, forEach->array);

}

static void forEachSetElement(ForEach *forEach,
							  Element *element)
{
	ClarityHeap *heap;

	heap = clarityGetHeap(forEach->clarity);
	clarityHeapRelease(heap, forEach->element);
	forEach->element = clarityHeapRetain(heap, element);
	forEach->index++;
}

static ForEach *forEachCreate(Clarity *clarity,
							  ClarityArray *array,
							  Element *element,
							  ClarityArrayForEachFunction function,
							  ClarityArrayForEachCallback callback,
							  void *data)
{
	ForEach *forEach;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	forEach = clarityHeapAllocate(heap,
								  sizeof(ForEach),
								  (ClarityHeapDestructor)forEachDestroy);

	forEach->index = 0;
	forEach->clarity = clarityHeapRetain(heap, clarity);
	forEach->array = clarityHeapRetain(heap, array);
	forEach->element = clarityHeapRetain(heap, element);
	forEach->data = clarityHeapRetain(heap, data);
	forEach->function = function;
	forEach->callback = callback;
	clarityHeapAutoRelease(heap, forEach);
	return forEach;
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

static void innerForEach(Clarity *clarity, ForEach *forEach)
{
	if (forEach->element != forEach->array->last) {
		forEach->function(forEach->element->data,
						  forEach->index,
						  forEach->array,
						  clarity);

		forEachSetElement(forEach,
						  forEach->element->next);

		clarityEnqueueEvent(clarity, (ClarityEvent)innerForEach, forEach);
	} else if (forEach->callback)
		forEach->callback(forEach->clarity, forEach->data);
}

void clarityArrayForEach(ClarityArray *array,
						 ClarityArrayForEachFunction function,
						 ClarityArrayForEachCallback callback,
						 void *data)
{
	Element *element;
	ForEach *forEach;

	element = array->first->next;
	forEach = forEachCreate(array->clarity,
							array,
							element,
							function,
							callback,
							data);

	clarityEnqueueEvent(array->clarity, (ClarityEvent)innerForEach, forEach);
}

static void innerMap(Clarity *clarity, Map *map)
{
	if (map->element != map->array->last) {
		void *newItem;

		newItem = map->function(map->element->data,
								map->index,
								map->array,
								clarity);

		clarityArrayPush(map->newArray, newItem);
		mapSetElement(map, map->element->next);
		clarityEnqueueEvent(clarity, (ClarityEvent)innerMap, map);
	} else
		map->callback(map->clarity, map->newArray, map->data);
}

void clarityArrayMap(ClarityArray *array,
					 ClarityArrayMapFunction function,
					 ClarityArrayMapCallback callback,
					 void *data)
{
	Element *element;
	Map *map;

	element = array->first->next;
	map = mapCreate(array->clarity,
					array,
					element,
					function,
					callback,
					data);

	clarityEnqueueEvent(array->clarity, (ClarityEvent)innerMap, map);
}

static void innerEvery(Clarity *clarity, Test *test)
{
	if (test->element != test->array->last && test->retVal) {
		test->retVal = test->function(test->element->data,
									  test->index,
									  test->array,
									  clarity);
		testSetElement(test, test->element->next);
		clarityEnqueueEvent(clarity, (ClarityEvent)innerEvery, test);
	} else
		test->callback(test->clarity, test->retVal, test->data);
}

void clarityArrayEvery(ClarityArray *array,
					   ClarityArrayTestFunction function,
					   ClarityArrayTestCallback callback,
					   void *data)
{
	Element *element;
	Test *test;

	element = array->first->next;
	test = testCreate(array->clarity,
					  array,
					  element,
					  function,
					  callback,
					  data);

	test->retVal = TRUE;
	clarityEnqueueEvent(array->clarity, (ClarityEvent)innerEvery, test);
}

static void innerSome(Clarity *clarity, Test *test)
{
	if (test->element != test->array->last && !test->retVal) {
		test->retVal = test->function(test->element->data,
									  test->index,
									  test->array,
									  clarity);
		testSetElement(test, test->element->next);
		clarityEnqueueEvent(clarity, (ClarityEvent)innerSome, test);
	} else
		test->callback(test->clarity, test->retVal, test->data);
}

void clarityArraySome(ClarityArray *array,
					  ClarityArrayTestFunction function,
					  ClarityArrayTestCallback callback,
					  void *data)
{
	Element *element;
	Test *test;

	element = array->first->next;
	test = testCreate(array->clarity,
					  array,
					  element,
					  function,
					  callback,
					  data);

	test->retVal = FALSE;
	clarityEnqueueEvent(array->clarity, (ClarityEvent)innerSome, test);
}

static void innerFilter(Clarity *clarity, Filter *filter)
{
	if (filter->element != filter->array->last) {
		Bool match;

		match = filter->function(filter->element->data,
								 filter->index,
								 filter->array,
								 clarity);

		if (match)
			clarityArrayPush(filter->newArray, filter->element->data);

		filterSetElement(filter, filter->element->next);
		clarityEnqueueEvent(clarity, (ClarityEvent)innerFilter, filter);
	} else
		filter->callback(filter->clarity, filter->newArray, filter->data);
}

void clarityArrayFilter(ClarityArray *array,
						ClarityArrayTestFunction function,
						ClarityArrayMapCallback callback,
						void *data)
{
	Element *element;
	Filter *filter;

	element = array->first->next;
	filter = filterCreate(array->clarity,
						  array,
						  element,
						  function,
						  callback,
						  data);

	clarityEnqueueEvent(array->clarity, (ClarityEvent)innerFilter, filter);
}
