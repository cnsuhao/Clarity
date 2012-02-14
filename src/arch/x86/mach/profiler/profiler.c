#include "Clarity.h"
#include <stdlib.h>

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
	char* dst8 = (char *)dstData;
	char* src8 = (char *)srcData;

	while (size--)
		*dst8++ = *src8++;
	return dstData;
}

Uint32 clarityStrLen(const char *cString)
{
	const char *s;

	for (s = cString; *s;)
		++s;
	return (Uint32)(s - cString);
}

Sint32 clarityStrCmp(const char *cString,
	const char *cString2)
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

void *clarityMemSet(void *s, Uint32 c, Uint32 n)
{
	unsigned char *us = s;
	unsigned char uc = c;

	while (n-- != 0)
		*us++ = uc;

	return s;
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

