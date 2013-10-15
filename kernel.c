#include "kernel.h"

TD *Active, Kernel;
Stack KernelStack;

void InitKernel(){
    //initialize Queues
    ReadyQ = CreateList(L_PRIORITY);
    BlockedQ = CreateList(L_WAITING);
    FreeQ = CreateList(UNDEF);

    int i = 0;
    for(i = 0; i < MAX_THREADS; i++){
        Descriptors[i].tid = MAX_THREADS;
    }

    Active = &Descriptors[CreateThread((uval32)(&Idle), 0, 1)];

#ifdef NATIVE
    InitTD(&Kernel, (uval32) SysCallHandler, (uval32) &(KernelStack.stack[STACKSIZE]), 0);
    Kernel.regs.sr = DEFAULT_KERNEL_SR;
#endif /* NATIVE */
}

void K_SysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2){
#ifdef NATIVE
    asm(".align 4; .global SysCallHandler; SysCallHandler:");
    uval32 sysMode = SYS_EXIT;
#endif

    RC returnCode;
    switch(type){
        case SYS_CREATE:
            returnCode = CreateThread(arg0, arg1, arg2);
            break;
        case SYS_DESTROY:
            returnCode = DestroyThread(arg0);
            break;
        case SYS_RESUME:
            returnCode = ResumeThread(arg0);
            break;
        case SYS_YIELD:
            returnCode = Yield();
            break;
        case SYS_SUSPEND:
            returnCode = Suspend();
            break;
        case SYS_PRIORITY:
            returnCode = ChangeThreadPriority(arg0, arg1);
            break;
        default:
            myprint("Invalid SysCall type\n");
            returnCode = RC_FAILED;
            break;
    }
#ifdef NATIVE
    asm volatile("ldw r8, %0" : : "m" (sysMode) : "r8");
    asm("trap");
#endif /* NATIVE */
}

/**
 * Creates a new thread that should start executing the procedure pointed to by pc.
 * Creating a new thread should be done by first allocating a stack at the user level
 * (using malloc() with a minimum size of 8K). At the kernel level, this
 * should cause a new thread descriptor to be allocated, its fields to be initialized
 * and the descriptor to be enqueued in the ReadyQ. If the new thread has higher
 * priority than the invoking thread then the invoking thread should yield the
 * processor to the new thread.
 * @return The thread Id of the new thread,
 * RESOURCE_ERROR of there are no thread descriptors available,
 * STACK_ERRROR if stackSize is not at least 8K large, and
 * PRIORITY_ERROR if priority is not in the range of valid priorities.
 */
ThreadId CreateThread(uval32 pc, uval32 sp, uval32 priority){

    TD* newThread = CreateTD();
    if(newThread == 0)
        return RC_FAILED;

    InitTD(newThread, pc, sp, priority);

    PriorityEnqueue(newThread, ReadyQ);

    myprint("CreateThread\n");
    return newThread->tid;
}

/**
 * Destroys the thread identified by tid. If tid is 0 or the same as that of the
 * invoking thread, then the invoking thread should be destroyed. (Note that this
 * means that no thread should have a tid of 0, otherwise no other thread could
 * destroy it). In the kernel, this can be achieved by removing the thread descriptor
 * from whatever queue it is in and adding it to the list of free descriptors. If the
 * Active thread is to be killed, then a new thread should be dispatched.
 * @return TID_ERROR if there is no thread corresponding to tid, or OK otherwise.
 */
RC DestroyThread(ThreadId tid){
    myprint("Destroy Thread");
    if(Active->tid = tid){
        Active = DequeueHead(ReadyQ);
    }
    DestroyTD(tid);
    return RC_SUCCESS;
}

/**
 * The invoking thread should yield the processor to the highest priority ready-to-run
 * thread with equal or higher priority. In the kernel, this is achieved by enqueueing
 * the Active thread onto the ReadyQ behind all threads of the same, or higher,
 * priority (remember that the higher a thread's Priority number, the lower its
 * priority), and dispatching the ready-to-run thread with the highest priority (which
 * happens to be at the head of the ReadyQ).
 * @return Should return OK.
 */
RC Yield(){
    myprint("Yield\n");
    if(!Active){
        myprint("No active thread\n");
        return 0;
    }
    PriorityEnqueue(Active, ReadyQ);
    Active = DequeueHead(ReadyQ);
    return RC_SUCCESS;
}

/**
 * The invoking thread should block until it is woken up again. This can be achieved
 * by enqueueing the Active thread onto BlockedQ, and dispatching the ready-to-run
 * thread with the highest priority.
 * @return Should return OK.
 */
RC Suspend(){
    myprint("Suspend");
    EnqueueAtHead(Active, BlockedQ);
    Active = DequeueHead(ReadyQ);
    return 0;
}

/**
 * Wakes up the thread identified by the tid and makes it ready to run. If that thread
 * has higher priority than the invoking thread then the invoking thread should yield
 * the processor.
 * @return Should return TID_ERROR if there is no thread with Id tid,
 * NOT_BLOCKED if the target thread is not blocked, and OK otherwise.
 */
RC ResumeThread(ThreadId tid){
    myprint("ResumeThread");
    TD* resumeThread = FindTD(tid, BlockedQ);
    DequeueTD(resumeThread);
    PriorityEnqueue(resumeThread, ReadyQ);
    return 0;
}

/**
 * Changes the priority of the target thread identified by tid to newPriority.
 * This can be achieved by setting the priority field of the thread descriptor to
 * newPriority. If the corresponding TD is in the ReadyQ, then remove and re-insert
 * it, so that the ReadyQ remains sorted according to Priority. If the target thread
 * now has higher priority than the invoking thread then the invoking thread should
 * yield the processor to the target thread.
 * @return Should return TID_ERROR if there is no thread
 * with Id tid, PRIORITY_ERROR if newPriority is not valid, and OK otherwise.
 */
RC ChangeThreadPriority(ThreadId tid, int newPriority){
    if(tid < MAX_THREADS){
        Descriptors[tid].priority = newPriority;
    }
    return 0;
}

void Idle(){
    myprint("Running idle thread\n");
    int i;
    while(1){
        myprint("CPU is idle\n");
        for(i = 0; i < MAX_THREADS; i++){
        }
        Yield();
    }
}
