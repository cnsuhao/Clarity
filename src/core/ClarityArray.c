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
#include "ClarityMacro.h"
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
	} else if (map->callback) {
		ClarityHeap *heap;

		map->callback(map->clarity, map->newArray, map->data);
		heap = clarityGetHeap(map->clarity);
		clarityHeapRelease(heap, map);
	}
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

void clarityArrayEvery(ClarityArray *array,
					   ClarityArrayTestFunction function,
					   ClarityArrayTestCallback callback,
					   void *data)
{
	UNUSED(array);
	UNUSED(function);
	UNUSED(callback);
	UNUSED(data);
	/*Bool retVal;
	Uint32 index;
	Element *item;

	retVal = TRUE;
	item = array->first->next;
	index = 1;

	while (item != array->last && retVal) {
		retVal = arrayFunction(item->next->data, index, array, array->clarity);
		index++;
		item = item->next;
	}
	return retVal;*/
}

void clarityArrayFilter(ClarityArray *array,
						ClarityArrayTestFunction function,
						ClarityArrayTestCallback callback,
						void *data)
{
	UNUSED(array);
	UNUSED(function);
	UNUSED(callback);
	UNUSED(data);
	/*Uint32 index;
	Element *item;
	ClarityArray *newArray;

	newArray = clarityArrayCreate(array->clarity);
	item = array->first->next;
	index = 1;

	while (item != array->last) {
		if (arrayFunction(item->next, index, array, array->clarity))
			clarityArrayPush(newArray, item->next->data);
		index++;
		item = item->next;
	}
	return newArray;*/
}

ClarityArray *clarityArrayConcat(ClarityArray *array1, ClarityArray *array2)
{
	UNUSED(array1);
	UNUSED(array2);
	/*TODO implement concat */
	return NULL;
}

ClarityArray *clarityArraySort(ClarityArray *array)
{
	UNUSED(array);
	/*TODO implement sort */
	return NULL;
}
