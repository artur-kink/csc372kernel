#ifndef _KERNEL_H_
#define _KERNEL_H_

#include <stdlib.h>
#include <assert.h>

#include "defines.h"
#include "list.h"
#include "user.h"

typedef struct type_STACK Stack;

struct type_STACK{
    uval8 stack[STACKSIZE];
};

LL *ReadyQ, *BlockedQ, *FreeQ;

extern TD *Active, Kernel;

ThreadId CreateThread(uval32 pc, uval32 stackSize, uval32 priority);

RC DestroyThread(ThreadId tid);
RC Yield();
RC Suspend();
RC ResumeThread(ThreadId tid);
RC ChangeThreadPriority(ThreadId tid, int newPriority);

void Idle(void);
void InitKernel();

void K_SysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
extern void SysCallHandler(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2);
#endif
