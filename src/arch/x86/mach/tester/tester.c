#include "Clarity.h"
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

int main(int argc, char *argv[])
{
	int i;
	int nbr = 1;
	Clarity *clarity = clarityHeapRetain(clarityCore());
	const char *entry = "Entry";
	const char **files = &entry;

	if (argc > 1) {
		files = (const char **)argv;
		nbr = argc;
	}

	for (i = 1; i < nbr; i++)
		clarityStart(clarity, files[i]);

	clarityStop(clarity);
	clarityHeapRelease(clarity);
	return 0;
}

