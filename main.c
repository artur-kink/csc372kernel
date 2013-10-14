#include "defines.h"
#include "list.c"
//#include "user.c"
#include "kernel.c"
#include "user.c"

#include <assert.h>
#include <stdlib.h>

#ifndef NATIVE
#include <stdio.h>
#endif /* NATIVE */
  
int main(void){   
  InitKernel();//Initialize all kernel data structures
  
  USERMODE;    //Switch to user mode 

  mymain();    //Now call what you would normally call main() 
  
  return 0;
}

#ifdef NATIVE

void myprint(char *text){
  
  while(*text != '\0') {
    
    if((*JTAG_UART_CONTROL)&0xffff0000 ) {
      *JTAG_UART_DATA = (*text++);
    }
  }
}

#else /* NATIVE */

void myprint(char *text){
  printf(text);
}

#endif /* NATIVE */
