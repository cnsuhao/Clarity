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
#ifndef __CLARITYCORE_H__
#define __CLARITYCORE_H__
#include "ClarityTypes.h"
#include "ClarityHeap.h"

typedef struct __ClarityCore ClarityCore;

typedef void(*ClarityEvent)(void *);
typedef Sint8(*ClarityComparator)(void *, void *);
typedef void(*ClarityDestructor)(void *);

typedef void*(*ClarityMemCpy)(ClarityCore *, void *, const void *, Uint32);
typedef void*(*ClarityMemSet)(ClarityCore *, void *, char, Uint32);
typedef Uint32(*ClarityStrLen)(ClarityCore *, const char *);
typedef Sint8(*ClarityStrCmp)(ClarityCore *, const char *, const char *);

void claritySetMemCpy(ClarityCore *, ClarityMemCpy);
void claritySetMemSet(ClarityCore *, ClarityMemSet);
void claritySetStrLen(ClarityCore *, ClarityStrLen);
void claritySetStrCmp(ClarityCore *, ClarityStrCmp);

void *clarityAllocate(ClarityCore *, Uint32);
void *clarityAllocateWithDestructor(ClarityCore *core, Uint32 size,
	ClarityDestructor destructor);

void *clarityAutoRelease(void *);
void clarityRelease(void *);
void clarityForceRelease(void *);
void *clarityRetain(void *);
void clarityCollectGarbage(ClarityCore *);

ClarityCore *clarityCore(void *);
void *clarityUndefined(void);
void *clarityGlobal(ClarityCore *);

void *clarityMemCpy(ClarityCore *, void *, const void *, Uint32);
void *clarityMemSet(ClarityCore *, void *, char, Uint32);
Uint32 clarityStrLen(ClarityCore *, const char *);
Sint8 clarityStrCmp(ClarityCore *, const char *, const char *);

ClarityCore *clarityCreate(ClarityEvent, ClarityHeap *);
void clarityEnqueueEvent(ClarityCore *, ClarityEvent, void *);
void clarityPushEvent(ClarityCore *, ClarityEvent, void *);
void clarityPushFile(ClarityCore *, void *);
void clarityStart(ClarityCore *);
void clarityStop(ClarityCore *);

#endif
