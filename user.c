#include "defines.h"
#include "list.h"
#include "user.h"
#include "main.h"

#ifndef NATIVE

#include "kernel.h" 

#endif /* NATIVE */

#include <stdlib.h>
#include <assert.h>

RC SysCall(SysCallType type, uval32 arg0, uval32 arg1, uval32 arg2){
    uval32 returnCode;

#ifdef NATIVE  
    uval32 sysMode = SYS_ENTER;
    asm volatile("ldw r8, %0\n\t"
	       "ldw r4, %1\n\t" 
	       "ldw r5, %2\n\t"
	       "ldw r6, %3\n\t"
	       "ldw r7, %4\n\t" 
	       "trap"
	       : : "m" (sysMode), "m" (type), "m" (arg0), "m" (arg1), "m" (arg2)
	       : "r4", "r5", "r6", "r7", "r8");  
#else /* NATIVE */
    returnCode = CreateThread(arg0, arg1, arg2); //Kernel system call - not normally accessible from user space
#endif /* NATIVE */
    return returnCode;
} 

void mymain(){ 
	myprint("Starting mymain\n");
    RC ret = SysCall(SYS_CREATE, mymain+12*4, 0, 1);
	myprint("Called CREATE\n");
	static int counter = 0;
    myprint("DONE\n");
	if(counter == 0){
		counter = 1;
		while(1){
			myprint("m1");
			SysCall(SYS_YIELD, 0, 0, 0);
		}
	}else{
		while(1){
			myprint("m2");
			SysCall(SYS_YIELD, 0, 0, 0);
		}
	}
}