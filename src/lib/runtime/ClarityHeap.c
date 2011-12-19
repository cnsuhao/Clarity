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
#include "ClarityHeap.h"
#include <stdlib.h>

typedef struct {
	Uint32 magic;
	ClarityHeapDestructor destructor;
	Sint32 refCount;
} Header;

typedef struct __AutoReleaseItem AutoReleaseItem;
struct __AutoReleaseItem {
	Header *header;
	AutoReleaseItem *next;
};

struct __ClarityHeap {
	AutoReleaseItem *autoReleasePool;
	Uint32 openAllocs;
	Uint32 maxAllocs;
};

typedef void(*Release)(ClarityHeap *heap, Header *header);

static Uint32 HEAP_MAGIC = 0xC0FFEE0D;
static AutoReleaseItem LAST_AUTO_RELEASE_ITEM;

static void pushAutoReleasePool(ClarityHeap *heap, Header *header)
{
	AutoReleaseItem *pool;

	pool = heap->autoReleasePool;
	heap->autoReleasePool = malloc(sizeof(AutoReleaseItem));
	heap->autoReleasePool->header = header;
	heap->autoReleasePool->next = pool;
}

static Header *popAutoReleasePool(ClarityHeap *heap)
{
	Header *header;
	AutoReleaseItem *item;

	item = heap->autoReleasePool;
	header = item->header;
	heap->autoReleasePool = item->next;
	free(item);
	return header;
}

static void initializeHeader(Header *header, ClarityHeapDestructor destructor)
{
	header->magic = HEAP_MAGIC;
	header->destructor = destructor;
	header->refCount = 1;
}

void *clarityHeapAllocate(ClarityHeap *heap,
						  Uint32 size,
						  ClarityHeapDestructor destructor)
{
	Header *header;

	UNUSED(heap);
	header = malloc(size + sizeof(Header));
	initializeHeader(header, destructor);
	heap->openAllocs++;
	heap->maxAllocs = MAX(heap->maxAllocs, heap->openAllocs);
	return (Uint8 *)header + sizeof(Header);
}

static Header *heapItemHeader(void *data)
{
	Header *retVal;

	retVal = NULL;

	if (data != NULL) {
		Header *header;

		header = (Header *)((Uint8 *)data - sizeof(Header));

		if (header->magic == HEAP_MAGIC)
			retVal = header;
	}
	return retVal;
}

static void clarityHeapFree(ClarityHeap *heap, Header *header)
{
	Uint8 *index;

	index = (Uint8 *)header;
	header->destructor(heap, &index[sizeof(Header)]);
	free(header);
	heap->openAllocs--;
}

static void innerRelease(ClarityHeap *heap, void *data, Release release)
{
	Header *header;

	header = heapItemHeader(data);

	if (header) {
		header->refCount--;

		if (header->refCount <= 0)
			release(heap, header);
	}
}

void clarityHeapAutoRelease(ClarityHeap *heap, void *data)
{
	innerRelease(heap, data, pushAutoReleasePool);
}

void clarityHeapRelease(ClarityHeap *heap, void *data)
{
	innerRelease(heap, data, clarityHeapFree);
}

void *clarityHeapRetain(ClarityHeap *heap, void *data)
{
	Header *header;

	UNUSED(heap);
	header = heapItemHeader(data);

	if (header)
		header->refCount++;
	return data;
}

void clarityHeapCollectGarbage(ClarityHeap *heap)
{

	while (heap->autoReleasePool != &LAST_AUTO_RELEASE_ITEM) {
		Header *header;

		header = popAutoReleasePool(heap);

		if (header->refCount <= 0)
			clarityHeapFree(heap, header);
	}
}

static void destroy(ClarityHeap *heap, void *data)
{
	UNUSED(data);
	clarityHeapCollectGarbage(heap);
}

ClarityHeap *clarityHeapCreate(void)
{
	Header *header;
	ClarityHeap *heap;

	heap = NULL;
	header = malloc(sizeof(ClarityHeap) + sizeof(Header));

	if (header) {
		Uint8 *index;

		initializeHeader(header, destroy);
		index = (Uint8 *)header;
		heap = (ClarityHeap *)&index[sizeof(Header)];
		heap->autoReleasePool = &LAST_AUTO_RELEASE_ITEM;
		heap->openAllocs = 1;
		heap->maxAllocs = heap->openAllocs;
	}
	return heap;
}
