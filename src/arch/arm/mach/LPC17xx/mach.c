#include "ClarityTypes.h"
#include <stdlib.h>

void *_sbrk(long incr)
{
	extern char heapStart;
	static char *heap_next = 0;
	char  *prev_heap_next;

	if (heap_next == NULL)
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
	char* dst8 = (char *)dstData;
	char* src8 = (char *)srcData;

	while (size--)
		*dst8++ = *src8++;
	return dstData;
}

void *clarityMemSet(void *data, char value,
	Uint32 size)
{
	if (data) {
		char *p;

		p = data;
		while (size--)
			*p++ = value;
	}

	return data;
}

Uint32 clarityStrLen(const char *cString)
{
	const char *s;

	for (s = cString; *s;)
		++s;
	return (Uint32)(s - cString);
}

Sint8 clarityStrCmp(const char *cString, const char *cString2)
{
	unsigned char uc1;
	unsigned char uc2;

	while (*cString != '\0' && *cString == *cString2) {
		cString++;
		cString2++;
	}

	uc1 = (*(unsigned char *) cString);
	uc2 = (*(unsigned char *) cString2);
	return ((uc1 < uc2) ? -1 : (uc1 > uc2));
}
