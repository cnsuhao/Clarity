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
#include "ClarityObject.h"
#include "ClarityDictionary.h"
#include "ClarityHeap.h"

struct __ClarityObject {
	Clarity *clarity;
	ClarityDictionary *members;
};

static void destroy(ClarityHeap *heap, ClarityObject *object)
{
	clarityHeapRelease(heap, object->clarity);
	clarityHeapRelease(heap, object->members);
}

void *clarityObjectGetMember(ClarityObject *object, ClarityString *name)
{
	return clarityDictionaryGetObject(object->members, name);
}

void clarityObjectSetMember(ClarityObject *object,
							ClarityString *name,
							void *member)
{
	if (clarityDictionaryGetObject(object->members, name))
		clarityDictionaryRemoveObject(object->members, name);
	clarityDictionarySetObject(object->members, name, member);
}

ClarityObject *clarityObjectCreate(Clarity *clarity)
{
	ClarityHeap *heap;
	ClarityObject *object;

	heap = clarityGetHeap(clarity);
	object = clarityHeapAllocate(heap, sizeof(ClarityObject),
									 (ClarityHeapDestructor)destroy);

	object->clarity = clarityHeapRetain(heap, clarity);
	object->members = clarityDictionaryCreate(
		clarity,
		(ClarityComparator)clarityStringCompare);

	object->members = clarityHeapRetain(heap, object->members);
	clarityHeapAutoRelease(heap, object);
	return object;
}
