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

typedef void*(*ClarityAlloc)(Uint32);
typedef void(*ClarityFree)(void *);

typedef struct {
	Uint32 magic;
	ClarityHeapDestructor destructor;
	ClarityHeap *heap;
	Sint32 refCount;
	Uint32 size;
	void *data;
} Header;

typedef struct __AutoReleaseItem AutoReleaseItem;
struct __AutoReleaseItem {
	Header *header;
	AutoReleaseItem *next;
};

struct __ClarityHeap {
	AutoReleaseItem *autoReleasePool;
	ClarityAlloc alloc;
	ClarityFree free;
};

typedef void(*Release)(ClarityHeap *heap, Header *header);

static const Uint32 HEAP_MAGIC = 0xC0FFEE0D;
static const AutoReleaseItem LAST_AUTO_RELEASE_ITEM = {NULL, NULL};

static void autoReleasePoolPush(ClarityHeap *heap, Header *header)
{
	AutoReleaseItem *newItem;

	newItem = clarityHeapAllocate(heap, sizeof(AutoReleaseItem));
	if (newItem) {
		AutoReleaseItem *pool;

		pool = heap->autoReleasePool;
		heap->autoReleasePool = newItem;
		heap->autoReleasePool->header = header;
		heap->autoReleasePool->next = pool;
	}
}

static Header *autoReleasePoolPop(ClarityHeap *heap)
{
	Header *header;
	AutoReleaseItem *item;

	item = heap->autoReleasePool;
	heap->autoReleasePool = item->next;
	header = item->header;
	clarityHeapRelease(item);
	return header;
}

static void autoReleasePoolDelete(ClarityHeap *heap, Header *header)
{
	AutoReleaseItem *item;
	AutoReleaseItem *next;
	AutoReleaseItem *prev;

	item = heap->autoReleasePool;
	prev = NULL;

	while (item != &LAST_AUTO_RELEASE_ITEM) {
		next = item->next;
		if (item->header == header) {
			if (prev)
				prev->next = next;
			else
				heap->autoReleasePool = next;
			clarityHeapRelease(item);
		} else {
			prev = item;
		}
		item = next;
		if (item == heap->autoReleasePool)
			prev = NULL;
	}
}

static void initializeHeader(ClarityHeap *heap, Header *header, Uint32 size,
	ClarityHeapDestructor destructor)
{
	header->magic = HEAP_MAGIC;
	header->destructor = destructor;
	header->size = size;
	header->refCount = 1;
	header->heap = heap;
}

static void *clarityHeapInnerAllocate(ClarityHeap *heap, ClarityAlloc alloc,
	Uint32 size, ClarityHeapDestructor destructor)
{
	Header *header;
	void *retVal;

	retVal = NULL;
	header = alloc(size + sizeof(Header));

	if (header) {
		initializeHeader(heap, header, size, destructor);
		retVal = &header->data;
	}
	return retVal;
}

void *clarityHeapAllocateWithDestructor(ClarityHeap *heap, Uint32 size,
	ClarityHeapDestructor destructor)
{
	return clarityHeapInnerAllocate(heap, heap->alloc, size, destructor);
}

static void emptyDestroy(void *data)
{
}

void *clarityHeapAllocate(ClarityHeap *heap, Uint32 size)
{
	return clarityHeapAllocateWithDestructor(heap, size, emptyDestroy);
}

static Header *heapItemHeader(void *data)
{
	Header *retVal;

	retVal = NULL;

	if (data != NULL) {
		Header *header;

		header = (Header *)
			((Uint8 *)data - sizeof(Header) + sizeof(data));

		if (header->magic == HEAP_MAGIC)
			retVal = header;
	}
	return retVal;
}

static void clarityHeapFree(ClarityHeap *heap, Header *header)
{
	ClarityFree free;

	free = heap->free;
	autoReleasePoolDelete(heap, header);
	header->destructor(&header->data);
	free(header);
}

static void innerRelease(void *data, Release release)
{
	Header *header;

	header = heapItemHeader(data);

	if (header) {
		header->refCount--;

		if (header->refCount == 0)
			release(header->heap, header);
	}
}

ClarityHeap *clarityHeap(void *data)
{
	ClarityHeap *retVal = NULL;
	Header *header;

	header = heapItemHeader(data);

	if (header)
		retVal = header->heap;
	return retVal;
}

void *clarityHeapAutoRelease(void *data)
{
	innerRelease(data, autoReleasePoolPush);
	return data;
}

void clarityHeapRelease(void *data)
{
	innerRelease(data, clarityHeapFree);
}

void clarityHeapForceRelease(void *data)
{
	Header *header;

	header = heapItemHeader(data);

	if (header) {
		header->refCount = 0;
		clarityHeapFree(header->heap, header);
	}
}

void *clarityHeapRetain(void *data)
{
	Header *header;

	header = heapItemHeader(data);

	if (header)
		header->refCount++;
	return data;
}

void clarityHeapCollectGarbage(ClarityHeap *heap)
{
	while (heap->autoReleasePool != &LAST_AUTO_RELEASE_ITEM) {
		Header *header;

		header = autoReleasePoolPop(heap);

		if (header->refCount == 0)
			clarityHeapFree(heap, header);
	}
}

static void heapDestroy(ClarityHeap *heap)
{
	clarityHeapCollectGarbage(heap);
}

ClarityHeap *clarityHeapCreate(void)
{
	ClarityHeap *heap;

	heap = clarityHeapInnerAllocate(NULL, clarityAlloc, sizeof(ClarityHeap),
		(ClarityHeapDestructor)heapDestroy);

	if (heap) {
		Header *header;

		header = heapItemHeader(heap);
		header->heap = heap;
		heap->autoReleasePool =
			(AutoReleaseItem *)&LAST_AUTO_RELEASE_ITEM;
		heap->alloc = clarityAlloc;
		heap->free = clarityFree;
	}
	return clarityHeapAutoRelease(heap);
}
