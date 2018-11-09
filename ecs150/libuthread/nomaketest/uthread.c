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
	uthread_t TID;
	void* stack;
} TCB;

void uthread_yield(void)
{	/* TODO Phase 2 */
	TCB *old, *new;
	//if there is no thread to switch to, then return error
	if(queue_length(schedulerReady) == 0)
		return -1;
	//else dequeue old and new threads, switch contexts, and queue them in new queues.
	queue_dequeue(schedulerRunning, old);
	queue_dequeue(schedulerReady, new);
	uthread_ctx_switch(old, new);
	queue_enqueue(schedulerRunning, new);
	queue_enqueue(schedulerReady, old);

}

uthread_t uthread_self(void)
{
	/* TODO Phase 2 */
	TCB *temp;
	uthread_t result;
	//check if schedulerRunning has anything running at the moment, if not then return error
	if(schedulerRunning == NULL)
		return -1;
	
	//grab the thread that is running by dequeuing it, and take its TID, then requeue it
	//theres probably a better way to do this, so if your able to do that, please do

	queue_dequeue(schedulerRunning, temp);
	result = *temp -> TID;
	queue_enqueue(schedulerRunning, temp);
	return result;
}

int uthread_create(uthread_func_t func, void *arg)
{	/* TODO Phase 2 */
	//if the schedulers are NULL, then initialize
	if(schedulerReady == NULL)
	{
		TCB first;
		first -> status = RUNNING;
		first -> context = malloc(sizeof(uthread_ctx_t));
		first -> TID = 0;
		first -> stack = NULL;
		schedulerReady = queue_create();
		schedulerBlocked = queue_create();
		schedulerZombie = queue_create();
		schedulerRunning = queue_create();
		queue_enqueue(&first);
	}

	//if we already hit the max number of threads we can create
	if(CurrentID == USHRT_MAX)
	{
		return -1;
	}


	TCB thread;
	thread -> context = malloc(sizeof(uthread_ctx_t));
	int ret;
	thread -> stack = uthread_ctx_alloc_stack;
	ret = uthread_ctx_init(thread -> context, thread->stack, func, arg);
	
	//fail cases in case of malloc failed or uthread_ctx_init fails
	if(ret == -1 || thread->context == NULL)
	{
		return -1;
	}
	//if we get to this point, then the thread is healthy and running, and we shoud add it to a queue.
	thread -> status = READY;
	thread -> TID = CurrentID;
	queue_enqueue(schedulerReady, &thread);
	currentID += 1;

	return 0;
}

void uthread_exit(int retval)
{
	TCB *dead, *new;
	/* TODO Phase 2 */
	//queue the dying thread into zombie, then do a context switch
	queue_dequeue(schedulerRunning, dead);
	queue_enqueue(schedulerZombie, dead);
	ueue_dequeue(schedulerReady, new);
	uthread_ctx_switch(dead, new);
	queue_enqueue(schedulerRunning, new);
}

int uthread_join(uthread_t tid, int *retval)
{	/* TODO Phase 2 */
	// while(1)
	// {
	// 	//check if there is another thread to run, if ready threads are 0, then break.
	// 	if(queue_length(schedulerReady == 0))
	// 		return 0;
	// 	uthread_yield();
	// }

	/* TODO Phase 3 */
	//if tid doest exists, then return error
	if(tid < 0 || tid == NULL || uthread_self() == tid || tid > CurrentID)
		return -1;
	int *i;
	do
	{	//searches zombie queue for tid, if its inside of it, then join values from zombie
		//this is unfinished
		queue_iterate(schedulerZombie, find_item, (void*)tid, (void**)&i);
		if(*i == 1)
		{

		}

	}while(*i == 0);

}

