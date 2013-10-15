#include "list.h"

TD *CreateTD(){
    TD *thread = 0;
    int i = 0;
    for(i = 0; i < MAX_THREADS; i++){
        if(Descriptors[i].tid == MAX_THREADS){
            thread = &Descriptors[i];
            thread->tid = i;
            break;
        }
    }
    
    if(thread){
        thread->link = 0;
        thread->priority = 0;
        thread->waittime = 0;
        thread->inlist = 0;
        thread->returnCode = 0;

        thread->regs.pc = 0;
        thread->regs.sp = 0;
        thread->regs.sr = 0;
    } else{
        myprint("Failed to allocate new thread\n");
    }
    return thread;
}

void InitTD(TD *td, uval32 pc, uval32 sp, uval32 priority){
    if(td != NULL){
        td->regs.pc = pc;
        td->regs.sp = sp;
        td->regs.sr = DEFAULT_THREAD_SR;
        td->priority = priority;
    } else{
        myprint("Tried to initialize NULL pointer\n");
    }
}

void DestroyTD(ThreadId tid){
    if(tid < MAX_THREADS)
        Descriptors[tid].tid = MAX_THREADS;
}

/**
 * Allocates and properly initializes a list structure and
 * Returns a pointer to it or null.
 */
LL * CreateList(ListType type){
    LL* list;
    list = malloc(sizeof(LL));
    if(list == NULL){
        free(list);
        return NULL;
    }
    list->type = type;
    list->head = NULL;
    return list;
}

/** 
 * Destroys list, whose pointer is passed in as an argument.
 * Returns 0 if successful, and -1 otherwise.
 */
RC DestroyList(LL *list){
    TD *curr, *next;
    if(list){
        curr = list->head;

        while(curr != NULL){
            next = curr->link;
            curr->link = NULL;
            curr->inlist = NULL;
            curr = next;
        }

        free(list);
        return 0;
    }

    return -1;
}

/**
 * Dequeues the TD at the head of list and
 * Returns a pointer to it, or else null.
 */
TD* DequeueHead(LL *list){
    TD *td;
    if(list){
        td = list->head;
        if(td != NULL){
            list->head = td->link;
            td->link = NULL;
            td->inlist = NULL;
            return td;
        } else{
            return NULL;
        }
    }
    return NULL;
}

/** if list is a priority list, then enqueues td in its proper location. 
 * Returns -1 if list is not a priority list and 0 otherwise. */
RC PriorityEnqueue(TD* td, LL* list){
    TD *curr, *prev;
    int priority;

    if(list->type == L_PRIORITY){
        priority = td->priority;
        curr = list->head;
        prev = NULL;

        if(curr != NULL){

            while(curr != NULL){
                if(priority >= curr->priority){
                    prev = curr;
                    curr = curr->link;
                } else{
                    if(prev != NULL){
                        prev->link = td;

                    } else{
                        list->head = td;
                    }
                    td->link = curr;
                    td->inlist = list;

                    return 0;
                }

            }

            /* highest priority of every item on list */
            prev->link = td;
            td->link = NULL;
            td->inlist = list;
            return 0;

        } else{ /* list is empty. */
            td->inlist = list;
            list->head = td;
            return 0;
        }
    }

    return -1;
}

/** enqueues td at the head of list if list is a LIFO list. 
  Returns 0 if OK and -1 otherwise.*/
RC EnqueueAtHead(TD *td, LL *list){
    if(list->type == L_LIFO){
        td->link = list->head;
        td->inlist = list;
        list->head = td;
        return 0;
    }

    return -1;
}

/** if list is a waiting list, then inserts td in its correct position assuming it should
 * wait for waittime. The waittime values of the other elements in the list should be
 * properly adjusted.
 * Return -1 if list is not a waiting list and 0 otherwise. */
RC WaitListEnqueue(TD *td, int waittime, LL *list){
    TD *curr;
    TD *prev;
    int totaltime;

    curr = list->head;
    prev = NULL;
    totaltime = 0;

    if(list->type == L_WAITING){

        if(curr == NULL){
            list->head = td;
            td->inlist = list;
            td->waittime = waittime;
            return 0;

        } else{

            while(curr != NULL){
                if(waittime > curr->waittime){
                    prev = curr;
                    curr = curr->link;
                    totaltime += curr->waittime;
                } else{

                    if(prev != NULL){
                        prev->link = td;
                        td->waittime = waittime + totaltime;
                    } else{
                        list->head = td;
                        td->waittime = waittime;
                    }

                    td->link = curr;
                    td->inlist = list;
                    totaltime += td->waittime;
                    break;
                }

            }

            /* update wait times */
            while(curr != NULL){
                curr->waittime += totaltime;
                totaltime += curr->waittime;
                curr = curr->link;
            }

            return 0;
        }
    }

    return -1;
}

/** searches list for a TD with a threadID td, and 
 * Returns a pointer to it or null otherwise. */
TD * FindTD(ThreadId tid, LL *list){
    TD *td;
    td = list->head;
    while(td != NULL){
        if(td->tid == tid){
            return td;
        }
        td = td->link;
    }

    return NULL;
}

/** dequeues td from whatever list it might be in, if it is in one.*/
RC DequeueTD(TD *td){
    TD *curr;
    LL *list;
    list = td->inlist;

    if(list){
        if(list->head == td){ /* td is the head of the list */
            list->head = td->link;

        } else{
            curr = list->head;
            while(curr != NULL){
                if(curr->link == td){
                    curr->link = td->link;
                }
            }
        }
        td->link = NULL;
        td->inlist = NULL;
        return 0;
    }

    return -1;
}
