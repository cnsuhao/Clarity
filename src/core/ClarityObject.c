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
#include "ClarityBooleanObject.h"
#include "ClarityFunctionObject.h"
#include "ClarityString.h"

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

static ClarityObject *prototype = NULL;

static void nodeDestroy(Node *node)
{
	clarityRelease(node->object);
	clarityRelease(node->left);
	clarityRelease(node->right);
}

static Node *nodeCreate(ClarityCore *core, const char *name,
	ClarityObject *object)
{
	Node *node;
	node = clarityAllocateWithDestructor(core, sizeof(Node),
		(ClarityDestructor)nodeDestroy);

	node->name = name;
	node->object = clarityRetain(object);
	node->left = NULL;
	node->right = NULL;
	return clarityAutoRelease(node);
}

static void objectDestroy(ClarityObject *object)
{
	clarityRelease(object->root);
	clarityRelease(object->innerData);
}

void *clarityObjectGetInnerData(ClarityObject *object)
{
	void *retVal = NULL;

	if (object)
		retVal = object->innerData;
	return retVal;
}

void clarityObjectLock(ClarityObject *object)
{
	if (object)
		object->locked = TRUE;
}

typedef ClarityObject *(*NodeApplier)(Node **, const char *,
	ClarityObject *);

static void *applyNode(ClarityObject *object, const char *name,
	ClarityObject *subObject, NodeApplier found, NodeApplier notFound)
{
	Node *node;
	Node **assignee;
	node = object->root;
	assignee = &object->root;

	while (node != NULL) {
		Sint8 compare;

		compare = clarityStrCmp(clarityCore(object), name, node->name);
		if (compare == 0)
			return found(&node, name, subObject);
		else if (compare > 0)
			assignee = &node->left;
		else
			assignee = &node->right;
		node = *assignee;
	}
	return notFound(assignee, name, subObject);
}

static ClarityObject *getObjectFound(Node **node, const char *name,
	ClarityObject *object)
{
	return (*node)->object;
}

static ClarityObject *getObjectNotFound(Node **node, const char *name,
	ClarityObject *object)
{
	return clarityUndefined();
}

ClarityObject *clarityObjectGetOwnMember(ClarityObject *object,
	const char *name)
{
	return applyNode(object, name, NULL,
		getObjectFound, getObjectNotFound);
}

ClarityObject *clarityObjectGetMember(ClarityObject *object, const char *name)
{
	ClarityObject *undefined = clarityUndefined();
	ClarityObject *retVal = undefined;

	if (object && name) {
		ClarityObject *prototype = object;

		while (prototype != undefined && retVal == undefined) {
			retVal = clarityObjectGetOwnMember(prototype, name);

			if (retVal == undefined)
				prototype = clarityObjectGetOwnMember(prototype, "prototype");
		}
	}
	return retVal;
}

static ClarityObject *setObjectFound(Node **node, const char *name,
	ClarityObject *object)
{
	clarityRelease((*node)->object);
	(*node)->object = clarityRetain(object);
	return (*node)->object;
}

static ClarityObject *setObjectNotFound(Node **node, const char *name,
	ClarityObject *object)
{
	*node = clarityRetain(nodeCreate(clarityCore(object), name, object));
	return (*node)->object;
}

ClarityObject *clarityObjectSetMember(ClarityObject *object, const char *name,
	ClarityObject *subObject)
{
	ClarityObject *retVal = clarityUndefined();

	if (object && name && subObject && !object->locked)
		retVal = applyNode(object, name, subObject,
			setObjectFound, setObjectNotFound);
	return retVal;
}

const char *clarityObjectTypeOf(ClarityObject *object)
{
	const char *retVal = "undefined";

	if (object)
		retVal = object->type;
	return retVal;
}

static ClarityObject *equals(ClarityObject *scope)
{
	ClarityObject *retVal = clarityUndefined();

	if (scope) {
		ClarityCore *core = clarityCore(scope);
		Bool equals = (clarityObjectGetMember(scope, "this") ==
			clarityObjectGetOwnMember(scope, "$1"));
		retVal = clarityBooleanObjectCreate(core, equals);
	}
	return retVal;
}

ClarityObject *clarityObjectPrototypeCreate(ClarityCore *core)
{
	if (!prototype) {
		prototype = clarityObjectCreateType(core, "object", NULL);

		clarityObjectSetMember(prototype, "equals",
			clarityFunctionObjectCreate(core,
				equals,
				clarityUndefined()));

		clarityObjectLock(prototype);
	}
	return prototype;
}

ClarityObject *clarityObjectCreateType(ClarityCore *core,
	const char *type, void *innerData)
{
	ClarityObject *object;

	object = clarityAllocateWithDestructor(core, sizeof(ClarityObject),
		(ClarityDestructor)objectDestroy);

	object->locked = FALSE;
	object->type = type;
	object->innerData = clarityRetain(innerData);
	object->root = NULL;

	return clarityAutoRelease(object);
}

ClarityObject *clarityObjectCreate(ClarityCore *core)
{
	ClarityObject *object = clarityObjectCreateType(core, "object", NULL);

	clarityObjectSetMember(object, "prototype",
		clarityObjectPrototypeCreate(core));

	return object;
}
