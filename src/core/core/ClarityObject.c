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
#include "ClarityCore.h"

typedef struct __Node Node;
struct __Node {
	ClarityObject *object;
	const char *name;
	Node *left;
	Node *right;
};

struct __ClarityObject {
	Node *root;
	const char *type;
	Bool locked;
	void *innerData;
};

static ClarityObject *gPrototype = 0;
static ClarityObject *gUndefined = 0;

void clarityObjectStaticInitializer(
	ClarityObject *prototype, ClarityObject *undefined)
{
	gUndefined = clarityHeapRetain(undefined);
	gPrototype = clarityHeapRetain(prototype);
}

void clarityObjectStaticRelease(void)
{
	clarityHeapRelease(gUndefined);
}

static void nodeDestroy(Node *node)
{
	if (node) {
		clarityHeapRelease(node->object);
		clarityHeapRelease(node->left);
		clarityHeapRelease(node->right);
	}
}

static Node *nodeCreate(ClarityHeap *heap, const char *name,
	ClarityObject *object)
{
	Node *node;
	node = clarityHeapAllocateWithDestructor(heap, sizeof(Node),
		(ClarityHeapDestructor)nodeDestroy);

	if (node) {
		node->name = name;
		node->object = clarityHeapRetain(object);
		node->left = 0;
		node->right = 0;
	}

	return clarityHeapAutoRelease(node);
}

static void objectDestroy(ClarityObject *object)
{
	if (object) {
		clarityHeapRelease(object->root);
		clarityHeapRelease(object->innerData);
	}
}

void *clarityObjectGetInnerData(ClarityObject *object)
{
	void *retVal = 0;

	if (object)
		retVal = object->innerData;
	return retVal;
}

void clarityObjectLock(ClarityObject *object)
{
	if (object)
		object->locked = 1;
}

typedef ClarityObject *(*NodeApplier)(Node **, const char *, ClarityObject *);

static void *applyNode(ClarityObject *object, const char *name,
	ClarityObject *subObject, NodeApplier found, NodeApplier notFound)
{
	ClarityObject *retVal = gUndefined;

	if (object) {
		Node *node;
		Node **assignee;
		node = object->root;
		assignee = &object->root;

		while (node != 0) {
			Sint32 compare = clarityStrCmp(name, node->name);

			if (compare == 0)
				return found(&node, name, subObject);
			else if (compare > 0)
				assignee = &node->left;
			else
				assignee = &node->right;
			node = *assignee;
		}
		retVal = notFound(assignee, name, subObject);
	}
	return retVal;
}

static ClarityObject *getObjectFound(Node **node, const char *name,
	ClarityObject *object)
{
	ClarityObject *retVal = gUndefined;

	if (*node)
		retVal = (*node)->object;
	return retVal;
}

static ClarityObject *getObjectNotFound(Node **node, const char *name,
	ClarityObject *object)
{
	return gUndefined;
}

ClarityObject *clarityObjectGetOwnMember(ClarityObject *object,
	const char *name)
{
	return applyNode(object, name, 0,
		getObjectFound, getObjectNotFound);
}

ClarityObject *clarityObjectGetMember(ClarityObject *object, const char *name)
{
	ClarityObject *retVal = gUndefined;

	if (object && name) {
		ClarityObject *prototype = object;

		while (prototype != gUndefined && retVal == gUndefined) {
			retVal = clarityObjectGetOwnMember(prototype, name);

			if (retVal == gUndefined)
				prototype = clarityObjectGetOwnMember(prototype,
					"prototype");
		}
	}
	return retVal;
}

static ClarityObject *setObjectFound(Node **node, const char *name,
		ClarityObject *object)
{
	ClarityObject *nodeObject = (*node)->object;

	if (object != nodeObject) {
		clarityHeapRelease(nodeObject);
		(*node)->object = clarityHeapRetain(object);
	}
	return (*node)->object;
}

static ClarityObject *setObjectNotFound(Node **node, const char *name,
		ClarityObject *object)
{
	*node = clarityHeapRetain(nodeCreate(clarityHeap(object),
		name, object));

	return (*node)->object;
}

ClarityObject *clarityObjectSetMember(ClarityObject *object, const char *name,
		ClarityObject *subObject)
{
	ClarityObject *retVal = gUndefined;

	if (object && object != subObject) {
		retVal = object;
		if (name && subObject && !object->locked)
			applyNode(object, name, subObject,
					setObjectFound, setObjectNotFound);
	}
	return retVal;
}

const char *clarityObjectTypeOf(ClarityObject *object)
{
	const char *retVal = "undefined";

	if (object)
		retVal = object->type;

	return retVal;
}

Bool clarityObjectIsTypeOf(ClarityObject *object, const char *type)
{
	Bool retVal = 0;

	if (type)
		retVal = (clarityStrCmp(
		clarityObjectTypeOf(object), type) == 0);

	return retVal;
}

ClarityObject *clarityObjectCreateType(ClarityHeap *heap,
	const char *type, void *innerData)
{
	ClarityObject *object;

	object = clarityHeapAllocateWithDestructor(heap, sizeof(ClarityObject),
			(ClarityHeapDestructor)objectDestroy);

	if (object) {
		object->locked = 0;
		object->type = type;
		object->innerData = clarityHeapRetain(innerData);
		object->root = 0;
	}

	return clarityHeapAutoRelease(object);
}

ClarityObject *clarityObjectCreate(ClarityHeap *heap)
{
	ClarityObject *object = clarityObjectCreateType(heap, "object", 0);

	clarityObjectSetMember(object, "prototype", gPrototype);

	return object;
}

