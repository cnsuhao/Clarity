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
#include "ClarityObject.h"
#include "ClarityDictionary.h"
#include "ClarityString.h"

struct __ClarityObject {
	ClarityCore *clarity;
	ClarityDictionary *members;
};

static void objectDestroy(ClarityObject *object)
{
	clarityRelease(object->members);
}

void *clarityObjectGetMember(ClarityObject *object, const char *cName)
{
	ClarityString *name;

	name = clarityStringCreate(clarityCore(object), cName);
	return clarityDictionaryGetObject(object->members, name);
}

void clarityObjectSetMember(ClarityObject *object,
							const char *cName,
							void *member)
{
	ClarityString *name;

	name = clarityStringCreate(clarityCore(object), cName);
	clarityDictionarySetObject(object->members, name, member);
}

ClarityObject *clarityObjectCreate(ClarityCore *clarity)
{
	ClarityObject *object;

	object = clarityAllocate(clarity,
							 sizeof(ClarityObject),
							 (ClarityDestructor)objectDestroy);

	object->members = clarityRetain(clarityDictionaryCreate(
		clarity,
		(ClarityComparator)clarityStringCompare));

	return clarityAutoRelease(object);
}
