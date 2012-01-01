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

typedef struct {
	Uint32 magic;
	ClarityHeapDestructor destructor;
	ClarityHeap *heap;
	void *context;
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
	Uint32 blockSize;
	void *address;
	ClarityAlloc alloc;
	ClarityFree free;
};

typedef void(*Release)(ClarityHeap *heap, Header *header);

static const Uint32 HEAP_MAGIC = 0xC0FFEE0D;
static const AutoReleaseItem LAST_AUTO_RELEASE_ITEM = {NULL, NULL};

static void autoReleasePoolPush(ClarityHeap *heap, Header *header)
{
	AutoReleaseItem *newItem;

	newItem = clarityHeapAllocate(heap,
								  NULL,
								  sizeof(AutoReleaseItem),
								  (ClarityHeapDestructor)NULL);
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

static void initializeHeader(ClarityHeap *heap,
							 void *context,
							 Header *header,
							 Uint32 size,
							 ClarityHeapDestructor destructor)
{
	header->magic = HEAP_MAGIC;
	header->destructor = destructor;
	header->size = size;
	header->refCount = 1;
	header->heap = heap;
	header->context = context;
}

static void *clarityHeapInnerAllocate(ClarityHeap *heap,
									  void *context,
									  ClarityAlloc alloc,
									  Uint32 size,
									  ClarityHeapDestructor destructor)
{
	Header *header;
	void *retVal;

	retVal = NULL;
	header = alloc(size + sizeof(Header));

	if (header) {
		initializeHeader(heap, context, header, size, destructor);
		retVal = &header->data;
	}
	return retVal;
}


void *clarityHeapAllocate(ClarityHeap *heap,
						  void *context,
						  Uint32 size,
						  ClarityHeapDestructor destructor)
{
	return clarityHeapInnerAllocate(heap,
									context,
									heap->alloc,
									size,
									destructor);
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
	ClarityFree free;

	free = heap->free;
	autoReleasePoolDelete(heap, header);

	if (header->destructor)
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

void *clarityHeapAutoRelease(void *data)
{
	innerRelease(data, autoReleasePoolPush);
	return data;
}

void clarityHeapRelease(void *data)
{
	innerRelease(data, clarityHeapFree);
}

void *clarityHeapRetain(void *data)
{
	Header *header;

	header = heapItemHeader(data);

	if (header)
		header->refCount++;
	return data;
}

void *clarityHeapGetContext(void *data)
{
	void *retVal;
	Header *header;

	retVal = NULL;
	header = heapItemHeader(data);

	if (header)
		retVal = header->context;
	return retVal;
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

static void *defaultAlloc(Uint32 size)
{
	/*TODO Implement default alloc, at address with size*/
	return NULL;
}

static void defaultFree(void *data)
{
	/*TODO Implement default free*/
}

static ClarityHeap *clarityHeapCreatePrivate(ClarityAlloc alloc,
											 ClarityFree free,
											 void *address,
											 Uint32 size,
											 Uint32 blockSize)
{
	ClarityHeap *heap;

	heap = clarityHeapInnerAllocate(NULL,
									NULL,
									alloc,
									sizeof(ClarityHeap),
									(ClarityHeapDestructor)heapDestroy);
	if (heap) {
		Header *header;

		header = heapItemHeader(heap);
		header->heap = heap;
		heap->autoReleasePool = (AutoReleaseItem *)&LAST_AUTO_RELEASE_ITEM;
		heap->alloc = alloc;
		heap->free = free;
		heap->address = address;
		heap->size = size;
		heap->blockSize = blockSize;
	}
	return clarityHeapAutoRelease(heap);
}

ClarityHeap *clarityHeapCreate(void *address, Uint32 size, Uint32 blockSize)
{
	return clarityHeapCreatePrivate(defaultAlloc,
									defaultFree,
									address,
									size,
									blockSize);
}

ClarityHeap *clarityHeapCreateExternal(ClarityAlloc alloc, ClarityFree free)
{
	return clarityHeapCreatePrivate(alloc, free, NULL, 0, 0);
}
