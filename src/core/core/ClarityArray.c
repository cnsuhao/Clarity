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

struct __ClarityArrayIterator {
	Element *element;
	ClarityArray *array;
};

static void clarityArrayIteratorDestroy(ClarityArrayIterator *arrayIterator)
{
	if (arrayIterator) {
		clarityHeapRelease(arrayIterator->element);
		clarityHeapRelease(arrayIterator->array);
	}
}

Bool clarityArrayIteratorHasItem(ClarityArrayIterator *arrayIterator)
{
	Bool retVal = FALSE;

	if (arrayIterator)
		retVal = arrayIterator->element != arrayIterator->array->last;
	return retVal;
}

void *clarityArrayIteratorNextItem(ClarityArrayIterator *arrayIterator)
{
	void *retVal = NULL;
	if (arrayIterator) {
		Element *element = arrayIterator->element->next;

		clarityHeapRelease(arrayIterator->element);
		arrayIterator->element = clarityHeapRetain(element);
		if (arrayIterator->element)
			retVal = arrayIterator->element->data;
	}
	return retVal;
}

ClarityArrayIterator *clarityArrayIteratorCreate(ClarityHeap *heap,
	ClarityArray *array)
{
	ClarityArrayIterator *arrayIterator = NULL;

	if (array) {
		arrayIterator = clarityHeapAllocateWithDestructor(heap,
			sizeof(ClarityArrayIterator),
			(ClarityHeapDestructor)clarityArrayIteratorDestroy);

		if (arrayIterator) {
			arrayIterator->array = clarityHeapRetain(array);
			arrayIterator->element =
				clarityHeapRetain(array->first);
		}
	}

	return clarityHeapAutoRelease(arrayIterator);
}

static void elementDestroy(Element *element)
{
	if (element)
		clarityHeapRelease(element->data);
}

static Element *elementCreate(ClarityHeap *heap, void *data)
{
	Element *element;

	element = clarityHeapAllocateWithDestructor(heap,
		sizeof(Element), (ClarityHeapDestructor)elementDestroy);

	if (element) {
		element->data = clarityHeapRetain(data);
		element->next = NULL;
		element->prev = NULL;
	}

	return clarityHeapAutoRelease(element);
}

static void arrayDestroy(ClarityArray *array)
{
	if (array) {
		Element *item;
		Element *next;

		item = array->first->next;

		while (item != array->last) {
			next = item->next;
			clarityHeapRelease(item);
			item = next;
		}
		clarityHeapRelease(array->last);
		clarityHeapRelease(array->first);
	}
}

ClarityArray *clarityArrayUnshift(ClarityArray *array, void *data)
{
	if (array && !array->locked && array->first && array->first->next) {
		Element *newElement;

		newElement = clarityHeapRetain(
			elementCreate(clarityHeap(array), data));
		if (newElement) {
			newElement->next = array->first->next;
			newElement->prev = array->first;
			newElement->next->prev = newElement;
			array->first->next = newElement;
			array->length++;
		}
	}
	return array;
}

void *clarityArrayShift(ClarityArray *array)
{
	void *retVal = NULL;

	if (array && array->first &&
		array->first->next &&
		array->first->next->next &&
		clarityArrayLength(array) &&
		!array->locked) {
		Element *element;

		element = array->first->next;
		array->first->next = element->next;
		array->first->next->prev = array->first;
		array->length--;
		retVal = clarityHeapRetain(element->data);
		clarityHeapRelease(element);
		clarityHeapAutoRelease(retVal);
	}
	return retVal;

}

ClarityArray *clarityArrayPush(ClarityArray *array, void *data)
{
	if (array && !array->locked &&
		array->last &&
		array->last->prev) {
		Element *newElement;

		newElement = clarityHeapRetain(
			elementCreate(clarityHeap(array), data));

		if (newElement) {
			newElement->prev = array->last->prev;
			newElement->next = array->last;
			array->last->prev->next = newElement;
			array->last->prev = newElement;
			array->length++;
		}
	}
	return array;
}

void *clarityArrayPop(ClarityArray *array)
{
	void *retVal;

	retVal = NULL;

	if (array && clarityArrayLength(array) &&
		!array->locked &&
		array->last &&
		array->last->prev &&
		array->last->prev->next) {
		Element *element;

		element = array->last->prev;
		array->last->prev = element->prev;
		array->last->prev->next = array->last;
		array->length--;
		retVal = clarityHeapRetain(element->data);
		clarityHeapRelease(element);
		clarityHeapAutoRelease(retVal);
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
	Uint32 retVal = 0;
	if (array)
		retVal = array->length;
	return retVal;
}

ClarityArray *clarityArrayCreate(ClarityHeap *heap)
{
	ClarityArray *array;

	array = clarityHeapAllocateWithDestructor(heap,
		sizeof(ClarityArray), (ClarityHeapDestructor)arrayDestroy);

	if (array) {
		array->length = 0;
		array->locked = FALSE;
		array->first = clarityHeapRetain(elementCreate(heap, NULL));
		array->last = clarityHeapRetain(elementCreate(heap, NULL));
		if (array->first && array->last) {
			array->first->next = array->last;
			array->last->prev = array->first;
		}
	}
	return clarityHeapAutoRelease(array);
}
