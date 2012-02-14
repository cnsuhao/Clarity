#include "ClarityTypes.h"
#include <stdlib.h>
#include <string.h>

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

