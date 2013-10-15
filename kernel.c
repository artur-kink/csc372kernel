#include "kernel.h"

TD *Active, Kernel;
Stack KernelStack;

void InitKernel() {
  // initialize Queues
  ReadyQ = CreateList(L_PRIORITY);
  BlockedQ = CreateList(L_WAITING);
  FreeQ = CreateList(UNDEF);

  LastCreatedId = 0;
  Active = CreateTD(++LastCreatedId);
  InitTD(Active, 0, 0, 1);  //Will be set with proper return registers on context switch

#ifdef NATIVE
  InitTD(&Kernel, (uval32) SysCallHandler, (uval32) &(KernelStack.stack[STACKSIZE]), 0);
  Kernel.regs.sr = DEFAULT_KERNEL_SR;
#endif /* NATIVE */
}



void K_SysCall( SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2) 
{ 
#ifdef NATIVE
  asm(".align 4; .global SysCallHandler; SysCallHandler:");
  uval32 sysMode = SYS_EXIT;
#endif

  RC returnCode ; 
  switch( type ) {
    case SYS_CREATE:
      returnCode = CreateThread( arg0, arg1, arg2 ) ;
      break ;
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
    default:
    //myprint("Invalid SysCall type\n");
    returnCode = RC_FAILED;
    break;
  } 
#ifdef NATIVE
  asm volatile("ldw r8, %0" : : "m" (sysMode): "r8");
  asm( "trap" );
#endif /* NATIVE */
}

RC CreateThread( uval32 pc, uval32 sp, uval32 priority ){

    TD* newThread = CreateTD(++LastCreatedId);
    if(newThread == 0)
        return RC_FAILED;
    
    InitTD(newThread, pc, sp, priority);

    PriorityEnqueue(newThread, ReadyQ);

    myprint("CreateThread\n");
    return RC_SUCCESS;
}

RC DestroyThread(ThreadId tid){
  myprint("Destroy Thread");
  return 0;
}

RC Yield(){
  myprint("Yield");
  return 0;
}

RC Suspend(){
  myprint("Suspend");
  return 0;
}

RC ResumeThread(ThreadId tid){
  myprint("ResumeThread");
  return 0;
}


void 
Idle() 
{ 
  /*
  int i; 
  while( 1 ) 
    { 
      myprint( "CPU is idle\n" ); 
      for( i = 0; i < MAX_THREADS; i++ ) 
	{ 
	} 
      Yield(); 
    } 
  */
} 
