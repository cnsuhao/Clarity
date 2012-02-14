#include "ClarityTypes.h"
#include <stdlib.h>
#include <string.h>

void *_sbrk(long incr)
{
	extern char heapStart;
	static char *heap_next = 0;
	char  *prev_heap_next;

	if (heap_next == 0)
		heap_next = &heapStart;

	prev_heap_next = heap_next;
	heap_next += incr;

	return (void *)prev_heap_next;
}

void *clarityAlloc(Uint32 size)
{
	return malloc(size);
}

void clarityFree(void *data)
{
	free(data);
}

void *clarityMemCpy(void *dstData,
	const void *srcData, Uint32 size)
{
	return memcpy(dstData, srcData, size);
}

Uint32 clarityStrLen(const char *cString)
{
	return strlen(cString);
}

Sint32 clarityStrCmp(const char *cString,
	const char *cString2)
{
	return strcmp(cString, cString2);
}

void *clarityMemSet(void *s, Uint32 c, Uint32 n)
{
	return memset(s, c, n);
}

