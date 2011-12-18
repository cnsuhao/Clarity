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

static void releaseElement(void *item,
						   Uint32 index,
						   ClarityArray *array,
						   Clarity *clarity)
{
	ClarityHeap *heap;

	UNUSED(index);
	UNUSED(array);
	heap = clarityGetHeap(clarity);
	clarityHeapRelease(heap, item);
}

static void destroy(ClarityHeap *heap, ClarityArray *array)
{
	clarityHeapRelease(heap, array->clarity);
	clarityArrayForEach(array, releaseElement);
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
		retVal = element->data;
		heap = clarityGetHeap(array->clarity);
		clarityHeapRelease(heap, element);
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
		retVal = element->data;
		heap = clarityGetHeap(array->clarity);
		clarityHeapRelease(heap, element);
	}
	return retVal;
}

Uint32 clarityArrayLength(ClarityArray *array)
{
	return array->length;
}

void clarityArrayForEach(ClarityArray *array,
						 ClarityArrayForEachFunction arrayFunction)
{
	Uint32 index;
	Element *item;

	item = array->first->next;
	index = 1;

	while (item != array->last) {
		arrayFunction(item->next->data, index, array, array->clarity);
		index++;
		item = item->next;
	}
}

ClarityArray *clarityArrayMap(ClarityArray *array,
							  ClarityArrayMapFunction arrayFunction)
{
	Uint32 index;
	Element *item;
	ClarityArray *newArray;

	newArray = clarityArrayCreate(array->clarity);
	item = array->first->next;
	index = 1;

	while (item != array->last) {
		void *newItem;

		newItem = arrayFunction(item->next->data, index, array, array->clarity);
		clarityArrayPush(newArray, newItem);
		index++;
		item = item->next;
	}

	return newArray;
}

Bool clarityArrayEvery(ClarityArray *array,
					   ClarityArrayTestFunction arrayFunction)
{
	Bool retVal;
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
	return retVal;
}

ClarityArray *clarityArrayFilter(ClarityArray *array,
								 ClarityArrayTestFunction arrayFunction)
{
	Uint32 index;
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
	return newArray;
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
