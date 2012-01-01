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
#ifndef __CLARITY_H__
#define __CLARITY_H__
#include "ClarityHeap.h"

typedef struct __Clarity Clarity;

typedef void(*ClarityEvent)(Clarity *, void *);
typedef Sint8(*ClarityComparator)(void *, void *);
typedef void(*ClarityDestructor)(void *);

typedef void*(*ClarityMemCpy)(Clarity *, void *, const void *, Uint32);
typedef void*(*ClarityMemSet)(Clarity *, void *, char, Uint32);
typedef Uint32(*ClarityStrLen)(Clarity *, const char *);
typedef Sint8(*ClarityStrCmp)(Clarity *, const char *, const char *);

void claritySetMemCpy(Clarity *, ClarityMemCpy);
void claritySetMemSet(Clarity *, ClarityMemSet);
void claritySetStrLen(Clarity *, ClarityStrLen);
void claritySetStrCmp(Clarity *, ClarityStrCmp);

void *clarityAllocate(Clarity *, Uint32, ClarityDestructor);
void *clarityAutoRelease(void *);
void clarityRelease(void *);
void *clarityRetain(void *);
void clarityCollectGarbage(Clarity *);

void *clarityMemCpy(Clarity *, void *, const void *, Uint32);
void *clarityMemSet(Clarity *, void *, char, Uint32);
Uint32 clarityStrLen(Clarity *, const char *);
Sint8 clarityStrCmp(Clarity *, const char *, const char *);

Clarity *clarityCreate(ClarityEvent, ClarityHeap *);
void clarityEnqueueEvent(Clarity *, ClarityEvent, void *);
void clarityPushEvent(Clarity *, ClarityEvent, void *);
void clarityStart(Clarity *);
void clarityStop(Clarity *);

#endif
