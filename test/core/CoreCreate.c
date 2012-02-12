#include "Clarity.h"
#include "ClarityHeap.h"
#include <assert.h>

void clarityEntry(ClarityObject *globalScope)
{
	clarityCore();
	clarityCore();
	clarityStart(NULL);
}
