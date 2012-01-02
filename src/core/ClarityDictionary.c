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
	Node *parent;
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

static Node *itemCreate(ClarityCore *core,
						void *key,
						void *object,
						void *parent)
{
	Node *node;
	node = clarityAllocate(core,
						   sizeof(Node),
						   (ClarityDestructor)itemDestroy);

	node->key = clarityRetain(key);
	node->object = clarityRetain(object);
	node->left = NULL;
	node->right = NULL;
	node->parent = parent;
	return clarityAutoRelease(node);
}

static void dictionaryDestroy(ClarityDictionary *dictionary)
{
	clarityRelease(dictionary->root);
}

static Node *getNode(ClarityDictionary *dictionary, void *key)
{
	Node *node;

	node = dictionary->root;

	while (node != NULL) {
		Sint8 compare;

		compare = dictionary->comparator(key, node->key);

		if (compare == 0)
			return node;
		else if (compare > 0)
			node = node->left;
		else
			node = node->right;
	}
	return NULL;
}

void *clarityDictionaryGetObject(ClarityDictionary *dictionary, void *key)
{
	Node *node;
	void *retVal;

	node = getNode(dictionary, key);
	retVal = NULL;

	if (node)
		retVal = node->object;
	return retVal;
}

void clarityDictionarySetObject(ClarityDictionary *dictionary,
								void *key,
								void *object)
{
	Node *node;
	Node *parent;
	Node **assignee;
	node = dictionary->root;
	assignee = &dictionary->root;
	parent = NULL;

	while (node != NULL) {
		Sint8 compare;

		compare = dictionary->comparator(key, node->key);

		if (compare == 0) {
			clarityRelease(node->object);
			node->object = clarityRetain(object);
			return;
		} else if (compare > 0)
			assignee = &node->left;
		else
			assignee = &node->right;
		parent = node;
		node = *assignee;
	}
	*assignee = itemCreate(clarityCore(dictionary), key, object, parent);
	*assignee = clarityRetain(*assignee);

}

ClarityDictionary *clarityDictionaryCreate(ClarityCore *core,
										   ClarityComparator comp)
{
	ClarityDictionary *dictionary;

	dictionary = clarityAllocate(core,
								 sizeof(ClarityDictionary),
								 (ClarityDestructor)dictionaryDestroy);

	dictionary->comparator = comp;
	dictionary->root = NULL;
	return clarityAutoRelease(dictionary);
}
