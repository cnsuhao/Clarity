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
#ifndef __CLARITYARRAY_H__
#define __CLARITYARRAY_H__
#include "ClarityCore.h"

typedef struct __ClarityArray ClarityArray;

typedef void(*ClarityArrayForEachFunction)(void *,
										   Uint32,
										   ClarityArray *);

typedef void *(*ClarityArrayMapFunction)(void *,
										 Uint32,
										 ClarityArray *);

typedef Bool(*ClarityArrayTestFunction)(void *,
										Uint32,
										ClarityArray *);

typedef void(*ClarityArrayForEachCallback)(void *);

typedef void *(*ClarityArrayMapCallback)(void *, void *);

typedef void(*ClarityArrayTestCallback)(Bool, void *);

ClarityArray *clarityArrayCreate(ClarityCore *);
void clarityArrayPush(ClarityArray *, void *);
void *clarityArrayPop(ClarityArray *);
void clarityArrayUnshift(ClarityArray *, void *);
void *clarityArrayShift(ClarityArray *);
Uint32 clarityArrayLength(ClarityArray *);

void clarityArrayForEach(ClarityArray *array,
						 ClarityArrayForEachFunction arrayFunction,
						 ClarityEvent callback,
						 void *data);

void clarityArrayMap(ClarityArray *array,
					 ClarityArrayMapFunction function,
					 ClarityArrayMapCallback callback,
					 void *data);

void clarityArrayEvery(ClarityArray *array,
					   ClarityArrayTestFunction function,
					   ClarityArrayTestCallback callback,
					   void *data);

void clarityArraySome(ClarityArray *array,
					  ClarityArrayTestFunction function,
					  ClarityArrayTestCallback callback,
					  void *data);

void clarityArrayFilter(ClarityArray *array,
						ClarityArrayTestFunction function,
						ClarityArrayMapCallback callback,
						void *data);

ClarityArray *clarityArrayConcat(ClarityArray *, ClarityArray *);
ClarityArray *clarityArraySort(ClarityArray *);

#endif
