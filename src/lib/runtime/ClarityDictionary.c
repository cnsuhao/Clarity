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
#include "ClarityHeap.h"

typedef struct __Node Node;
struct __Node {
	void *object;
	void *key;
	Node *left;
	Node *right;
	Node *parent;
};

struct __ClarityDictionary {
	Clarity *clarity;
	Node *root;
	ClarityDictionaryKeyComparator comparator;
};

static void itemDestroy(ClarityHeap *heap, Node *node)
{
	clarityHeapRelease(heap, node->key);
	clarityHeapRelease(heap, node->object);
	clarityHeapRelease(heap, node->left);
	clarityHeapRelease(heap, node->right);
}

static Node *itemCreate(Clarity *clarity, void *key, void *object, void *parent)
{
	Node *node;
	ClarityHeap *heap;

	heap = clarityGetHeap(clarity);
	node = clarityHeapAllocate(heap,
							   sizeof(Node),
							   (ClarityHeapDestructor)itemDestroy);

	node->key = clarityHeapRetain(heap, key);
	node->object = clarityHeapRetain(heap, object);
	node->left = NULL;
	node->right = NULL;
	node->parent = parent;
	clarityHeapAutoRelease(heap, node);
	return node;
}

static void destroy(ClarityHeap *heap, ClarityDictionary *dictionary)
{
	clarityHeapRelease(heap, dictionary->clarity);
	clarityHeapRelease(heap, dictionary->root);
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
		else if (compare > 1)
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

void clarityDictionaryRemoveObject(ClarityDictionary *dictionary, void *key)
{
	Node *node;

	node = getNode(dictionary, key);

	if (node) {
		ClarityHeap *heap;

		heap = clarityGetHeap(dictionary->clarity);

		if (node->left == NULL && node->right == NULL)
			clarityHeapRelease(heap, node);

		/*TODO release items with children */
	}
}

void clarityDictionarySetObject(ClarityDictionary *dictionary,
								void *key,
								void *object)
{
	Node *node;
	Node *parent;
	Node **assignee;
	ClarityHeap *heap;

	heap = clarityGetHeap(dictionary->clarity);
	node = dictionary->root;
	assignee = &node;
	parent = NULL;

	while (node != NULL) {
		Sint8 compare;

		compare = dictionary->comparator(key, node->key);

		if (compare == 0)
			return;
		else if (compare > 1)
			assignee = &node->left;
		else
			assignee = &node->right;
		parent = node;
		node = *assignee;
	}
	*assignee = itemCreate(dictionary->clarity, key, object, parent);
	*assignee = clarityHeapRetain(heap, *assignee);

}

ClarityDictionary *clarityDictionaryCreate(Clarity *clarity,
										   ClarityDictionaryKeyComparator comp)
{
	ClarityHeap *heap;
	ClarityDictionary *dictionary;

	heap = clarityGetHeap(clarity);
	dictionary = clarityHeapAllocate(heap, sizeof(ClarityDictionary),
									 (ClarityHeapDestructor)destroy);

	dictionary->clarity = clarityHeapRetain(heap, clarity);
	dictionary->comparator = comp;
	dictionary->root = NULL;
	clarityHeapAutoRelease(heap, dictionary);
	return dictionary;
}
