#include "Clarity.h"
#include <assert.h>
#include <string.h>

#define TEST_VALUE "test string"

static ClarityObject *clarityEntry(ClarityObject *globalScope)
{
	ClarityHeap *heap = clarityHeap(globalScope);
	Uint32 length;
	ClarityObject *parameters;
	ClarityObject *object;

	object = clarityStringObjectCreate(heap, TEST_VALUE);
	parameters = clarityObjectCreate(heap);
	clarityObjectSetOwnMember(parameters, "this", object);
	length = clarityNumberObjectGetValue(
		clarityFunctionObjectCall(
		clarityObjectGetMember(object, "length"), parameters));
	assert(strlen(TEST_VALUE) == length);
	return clarityObjectCreate(heap);
}

static void init(void) __attribute__((unused, constructor));
static void init(void)
{
	clarityRegisterFile(clarityCore(),
		CLARITY_FILE, (ClarityFileInit)clarityEntry);
}
