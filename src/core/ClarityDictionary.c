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
#include "ClarityDictionary.h"

typedef struct __Node Node;
struct __Node {
	void *object;
	void *key;
	Node *left;
	Node *right;
};

struct __ClarityDictionary {
	Node *root;
	ClarityComparator comparator;
};

static void itemDestroy(Node *node)
{
	clarityRelease(node->key);
	clarityRelease(node->object);
	clarityRelease(node->left);
	clarityRelease(node->right);
}

static Node *itemCreate(ClarityCore *core, void *key, void *object)
{
	Node *node;
	node = clarityAllocateWithDestructor(core, sizeof(Node),
		(ClarityDestructor)itemDestroy);

	node->key = clarityRetain(key);
	node->object = clarityRetain(object);
	node->left = NULL;
	node->right = NULL;
	return clarityAutoRelease(node);
}

static void dictionaryDestroy(ClarityDictionary *dictionary)
{
	clarityRelease(dictionary->root);
}

typedef void*(*NodeApplier)(Node **, void *, void *);

static void *applyNode(ClarityDictionary *dictionary, void *key, void *object,
	NodeApplier found, NodeApplier notFound)
{
	Node *node;
	Node **assignee;
	node = dictionary->root;
	assignee = &dictionary->root;

	while (node != NULL) {
		Sint8 compare;

		compare = dictionary->comparator(key, node->key);

		if (compare == 0)
			return found(&node, key, object);
		else if (compare > 0)
			assignee = &node->left;
		else
			assignee = &node->right;
		node = *assignee;
	}
	return notFound(assignee, key, object);
}

static void *getObjectFound(Node **node, void *key, void *object)
{
	return (*node)->object;
}

static void *getObjectNotFound(Node **node, void *key, void *object)
{
	return NULL;
}

void *clarityDictionaryGetObject(ClarityDictionary *dictionary, void *key)
{
	return applyNode(dictionary, key, NULL, getObjectFound, getObjectNotFound);
}

static void *setObjectFound(Node **node, void *key, void *object)
{
	clarityRelease((*node)->object);
	(*node)->object = clarityRetain(object);
	return *node;
}

static void *setObjectNotFound(Node **node, void *key, void *object)
{
	*node = clarityRetain(itemCreate(clarityCore(key), key, object));
	return *node;
}

void clarityDictionarySetObject(ClarityDictionary *dictionary, void *key,
	void *object)
{
	applyNode(dictionary, key, object, setObjectFound, setObjectNotFound);
}

ClarityDictionary *clarityDictionaryCreate(ClarityCore *core,
	ClarityComparator comp)
{
	ClarityDictionary *dictionary;

	dictionary = clarityAllocateWithDestructor(core, sizeof(ClarityDictionary),
		(ClarityDestructor)dictionaryDestroy);

	dictionary->comparator = comp;
	dictionary->root = NULL;
	return clarityAutoRelease(dictionary);
}
