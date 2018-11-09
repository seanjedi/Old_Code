#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

//max amount of threads that can exist
#define USHRT_MAX 0xFFFFU

queue_t schedulerReady, schedulerRunning, schedulerBlocked, schedulerZombie;
uthread_t CurrentID = 1;;

typedef enum {
	READY,
	ZOMBIE,
	RUNNING,
	BLOCKED
} thread_status;

/* TODO Phase 2 */
typedef struct TCB {
	uthread_ctx_t *context;
	thread_status status;
	uthread_t TID, joinTID;
	void* stack;
} TCB;

TCB *main_TCB = NULL;


void uthread_yield(void)
{	// TODO Phase 2 
	void* a = (void*)malloc(sizeof(void*));
	void* b = (void*)malloc(sizeof(void*));

	TCB* oldTCB = (TCB*)malloc(sizeof(TCB*));
	oldTCB -> context = malloc(sizeof(uthread_ctx_t));
	TCB* newTCB = (TCB*)malloc(sizeof(TCB*));
	newTCB -> context = malloc(sizeof(uthread_ctx_t));


	//if there is no thread to switch to, then return error
	if(!(queue_length(schedulerReady) == 0)) {
		//\\return -1; void return, can't return -1 int
	//else dequeue old and new threads, switch contexts, and queue them in new queues.   

	queue_dequeue(schedulerRunning, a);
	queue_dequeue(schedulerReady, b);
	TCB* oldTCB = a;
	TCB* newTCB = b;

	uthread_ctx_switch(oldTCB->context, newTCB->context);

	queue_enqueue(schedulerRunning, &newTCB);

	queue_enqueue(schedulerReady, &oldTCB);
	main_TCB = newTCB; //not needed?

	}
}

uthread_t uthread_self(void)
{
	return main_TCB->TID;
}

int uthread_init() {
	main_TCB = (TCB*)malloc(sizeof(TCB*));
	if(main_TCB == NULL) {
		return -1;
	}
	main_TCB -> status = RUNNING;
	main_TCB -> context = malloc(sizeof(uthread_ctx_t));
	if(main_TCB->context == NULL) {
		return -1;
	}
	main_TCB -> TID = 0;
	main_TCB -> stack = NULL;
	schedulerReady = queue_create();
	schedulerZombie = queue_create();
	schedulerRunning = queue_create();
	schedulerBlocked = queue_create();
		printf("main_thread%p\n",main_TCB->context);

	queue_enqueue(schedulerRunning, main_TCB);
	queue_dequeue(schedulerRunning, (void**)main_TCB);
			printf("main%p\n",main_TCB->context);

	queue_enqueue(schedulerRunning, main_TCB);
	return 0;
}

int uthread_create(uthread_func_t func, void *arg)
{
	//if the schedulers are NULL, then initialize
	//no current thread?
	int ret = 0;
	if(schedulerReady == NULL)
	{
		ret = uthread_init();
		if(ret == -1) {
			return -1;
		}
	}

	//if we already hit the max number of threads we can create
	if(CurrentID == USHRT_MAX)
	{
		return -1;
	}

	TCB *TCB_thread = NULL;
	TCB_thread = malloc(sizeof(TCB));
	if(TCB_thread == NULL) {
		return -1;
	}
	TCB_thread -> context = malloc(sizeof(uthread_ctx_t));
	if(TCB_thread->context == NULL) {
		return -1;
	}

	void* st = NULL;
	st = uthread_ctx_alloc_stack();
	TCB_thread -> stack = st;
	ret = uthread_ctx_init(TCB_thread->context, TCB_thread->stack, func, arg);
	
	//fail cases in case of malloc failed or uthread_ctx_init fails
	if(ret == -1 || TCB_thread->context == NULL)
	{
		return -1;
	}
	//if we get to this point, then the thread is healthy and running, and we shoud add it to a queue.
	TCB_thread -> status = READY;
	TCB_thread -> TID = CurrentID;
	queue_enqueue(schedulerReady, TCB_thread);

	CurrentID += 1;

	return 0;
}

// static int find_item(queue_t q, void *data, void *arg)
// {
//     int *a = (int*)data;
//     int match = (intptr_t)arg;

 
//     if (*a == match)
//         return 1;

//     return 0;
// }

void uthread_exit(int retval)
{
/*
	//do you even need dead_TCB, new_TCB? for uthread_ctx_switch?
	void *deadV = NULL;
	void *newV = NULL;

	TCB *dead_TCB, *new_TCB;
	 TODO Phase 2 
	//queue the dying thread into zombie, then do a context switch
	queue_dequeue(schedulerRunning, deadV);
	dead_TCB = deadV;
	queue_enqueue(schedulerZombie, dead_TCB);
	queue_dequeue(schedulerReady, newV);
	new_TCB = newV;
	uthread_ctx_switch(dead_TCB->context, new_TCB->context);
	queue_enqueue(schedulerRunning, new_TCB); */

	main_TCB->status = ZOMBIE;
 
	TCB* temp = main_TCB;
	// TCB* found;
	// uthread_t tid;
	// tid = temp->TID;
	int i;
	i=0;

	//search blocked scheduler if there is a thread waiting for us
	//if thread is waiting, then switch back to that thread so that it
	//could wakeup and collect us
	// i = queue_iterate(schedulerBlocked, find_item, (void*)tid, (void**)found);
	if(i == 1)
	{
		// uthread_ctx_switch(temp->context, found->context);
	}else{
		//else enqueue old TCB into a zombie scheduler
		//and yield until a papa wants us
	queue_enqueue(schedulerZombie, temp);
	uthread_yield();
	}
	
 
 
}



int uthread_join(uthread_t tid, int *retval)
{	/* TODO Phase 2 */
	 while(1)
	 {
	printf("The value of p is: %d\n", 7);   

	 	//check if there is another thread to run, if ready threads are 0, then break.
	 	if(queue_length(schedulerReady) == 0)
	 		return 0;
	 	uthread_yield();
	 }

	// //TODO Phase 3 
	// //if tid doesnt exists, then return error
	// //tid == NULL can't happen cause tid is an int
	// if(tid < 0 || uthread_self() == tid || tid > CurrentID)
	// 	return -1;
	// int i;
	// TCB *temp;
	// do
	// {	//searches zombie queue for tid, if its inside of it, then join values from zombie
	// 	//this is unfinished
	// 	//void* tid? maybe context? why ints
	// 	i = queue_iterate(schedulerZombie, find_item, (void*)tid, (void**)temp);
	// 	if(i == 1)
	// 	{
	// 		//copy contents, then destroy old one
	// 		main_TCB -> stack = temp -> stack;
	// 		uthread_ctx_destroy_stack(temp);
	// 		queue_delete(schedulerZombie, temp);
	// 	}else{
	// 		main_TCB -> joinTID = TID;
	// 		queue_enqueue(schedulerBlocked, main_TCB);
	// 		uthread_yield();
	// 	}

	// }while(i == 0);

}

