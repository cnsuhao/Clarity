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
#include "ClarityHeap.h"

typedef struct {
	Uint32 magic;
	ClarityHeapDestructor destructor;
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
	Uint32 size;
	void *address;
	ClarityAlloc alloc;
	ClarityFree free;
	Uint32 openAllocations;
	Uint32 maxAllocations;
	Uint32 openAllocated;
	Uint32 maxAllocated;
};

typedef void(*Release)(ClarityHeap *heap, Header *header);

static const Uint32 HEAP_MAGIC = 0xC0FFEE0D;
static const AutoReleaseItem LAST_AUTO_RELEASE_ITEM = {NULL, NULL};

static void autoReleaseDestroy(ClarityHeap *heap, void *data)
{
	UNUSED(heap);
	UNUSED(data);
}

static void autoReleasePoolPush(ClarityHeap *heap, Header *header)
{
	AutoReleaseItem *newItem;

	newItem = clarityHeapAllocate(heap,
								  sizeof(AutoReleaseItem),
								  (ClarityHeapDestructor)autoReleaseDestroy);
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
	clarityHeapRelease(heap, item);
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
			if (item == heap->autoReleasePool)
				heap->autoReleasePool = next;
			else if (prev)
				prev->next = next;
			clarityHeapRelease(heap, item);
		} else {
			prev = item;
		}
		item = next;
		if (item == heap->autoReleasePool)
			prev = NULL;
	}
}

static void initializeHeader(Header *header,
							 Uint32 size,
							 ClarityHeapDestructor destructor)
{
	header->magic = HEAP_MAGIC;
	header->destructor = destructor;
	header->size = size;
	header->refCount = 1;
}

void *clarityHeapAllocate(ClarityHeap *heap,
						  Uint32 size,
						  ClarityHeapDestructor destructor)
{
	Header *header;
	void *retVal;

	UNUSED(heap);
	retVal = NULL;
	header = heap->alloc(size + sizeof(Header));

	if (header) {
		initializeHeader(header, size, destructor);
		heap->openAllocated += size + sizeof(Header);
		heap->maxAllocated = MAX(heap->maxAllocated,
								 heap->openAllocated);

		heap->openAllocations++;
		heap->maxAllocations = MAX(heap->maxAllocations,
								   heap->openAllocations);
		retVal = &header->data;
	}
	return retVal;
}

static Header *heapItemHeader(void *data)
{
	Header *retVal;

	retVal = NULL;

	if (data != NULL) {
		Header *header;

		header = (Header *)((Uint8 *)data - sizeof(Header) + sizeof(data));

		if (header->magic == HEAP_MAGIC)
			retVal = header;
	}
	return retVal;
}

static void clarityHeapFree(ClarityHeap *heap, Header *header)
{
	header->destructor(heap, &header->data);
	heap->openAllocations--;
	heap->openAllocated -= (header->size + sizeof(Header));
	autoReleasePoolDelete(heap, header);
	heap->free(header);
}

static void innerRelease(ClarityHeap *heap, void *data, Release release)
{
	Header *header;

	header = heapItemHeader(data);

	if (header) {
		header->refCount--;

		if (header->refCount == 0)
			release(heap, header);
	}
}

void clarityHeapAutoRelease(ClarityHeap *heap, void *data)
{
	innerRelease(heap, data, autoReleasePoolPush);
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

		header = autoReleasePoolPop(heap);

		if (header->refCount == 0)
			clarityHeapFree(heap, header);
	}
}

static void destroy(ClarityHeap *heap, void *data)
{
	UNUSED(data);
	clarityHeapCollectGarbage(heap);
}

static void *defaultAlloc(Uint32 size)
{
	/*TODO Implement default alloc, at address with size*/
	UNUSED(size);
	return NULL;
}

static void defaultFree(void *data)
{
	/*TODO Implement default free*/
	UNUSED(data);
}

static ClarityHeap *clarityHeapCreatePrivate(ClarityAlloc alloc,
											 ClarityFree free,
											 void *address,
											 Uint32 size)
{
	Header *header;
	ClarityHeap *heap;

	heap = NULL;
	header = alloc(sizeof(ClarityHeap) + sizeof(Header));

	if (header) {
		initializeHeader(header, sizeof(ClarityHeap), destroy);
		heap = (ClarityHeap *)&header->data;
		heap->autoReleasePool = (AutoReleaseItem *)&LAST_AUTO_RELEASE_ITEM;
		heap->openAllocations = 1;
		heap->alloc = alloc;
		heap->free = free;
		heap->openAllocated = sizeof(ClarityHeap) + sizeof(Header);
		heap->maxAllocations = heap->openAllocations;
		heap->maxAllocated = heap->openAllocated;
		heap->address = address;
		heap->size = size;
	}
	return heap;
}

ClarityHeap *clarityHeapCreate(void *address, Uint32 size)
{
	return clarityHeapCreatePrivate(defaultAlloc, defaultFree, address, size);
}

ClarityHeap *clarityHeapCreateExternal(ClarityAlloc alloc, ClarityFree free)
{
	return clarityHeapCreatePrivate(alloc, free, NULL, 0);
}
