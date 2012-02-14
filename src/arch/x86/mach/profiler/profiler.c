#include "Clarity.h"
#include <stdlib.h>
#include <string.h>

#define ITERATIONS 1000

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

int main(int argc, char *argv[])
{
	int i;
	int j;
	int nbr = 1;
	Clarity *clarity = clarityHeapRetain(clarityCore());
	const char *entry = "Entry";
	const char **files = &entry;

	if (argc > 1) {
		files = (const char **)argv;
		nbr = argc;
	}

	for (j = 0; j < ITERATIONS; j++) {
		for (i = 1; i < nbr; i++)
			clarityStart(clarity, files[i]);
	}

	clarityStop(clarity);
	clarityHeapRelease(clarity);
	return 0;
}

