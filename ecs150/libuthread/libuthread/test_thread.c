#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "uthread.h"
#include "queue.h"
#include "context.h"

//max amount of threads that can exist
#define USHRT_MAX 0xFFFFU


//will probably create a node a lot, create a helper for it?


#define HZ 100

void preempt_disable(void)
{
	/* TODO Phase 4 */
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
}

void preempt_start(void)
{
	/* TODO Phase 4 */
}

typedef struct node {
	void* data;
	struct node* next;
} node;

struct queue {
	int length;
	struct node* head;
	struct node* tail;
	/* TODO Phase 1 */
};


queue_t queue_create(void)
{
	queue_t new_queue = NULL;
	new_queue = (queue_t)malloc(sizeof(queue_t));

	if(new_queue == NULL) {
		fprintf(stderr, "malloc error, memory not available");
		//dont return because then it'll be all messed up
		exit(0);
	}
	new_queue->length = 0;
	new_queue->head = new_queue->tail = NULL;
	return new_queue;
	/* TODO Phase 1 */
}


//what if you swap to an empty queue, does the OS make the queue_t queue zombie and ok? and that would be O(1)?
int queue_destroy(queue_t queue)
{
	//if queue is null it won't go to queue->length and cause an error because or would just look at first and it's true so it'll be done
	if(queue == NULL || queue->length != 0) {
	return -1;
	}
	free(queue);
	return 0;
	/* TODO Phase 1 */
}


int queue_delete(queue_t queue, void *data)
{
	//if empty don't search of course
	if(queue == NULL || queue->length == 0 || data == NULL ) {
		fprintf(stderr, "delete NULL or empty parameter error");
		return -1;
	}
	
	//**use queue_iterate? maybe, haven't written yet so I'll see how it works later
	//iterate through until found, set to data, return;
	//use prev so when you delete then you connect the queue list again
	node* iterator = queue->head;
	node* prev = NULL;
	while(iterator != NULL) {
		//compare pointers right? not the data, just the address
		if(iterator->data == data) {
			prev->next = iterator->next;
			free(iterator);
			return 0;
		}
		//else continue
		prev = iterator;
		iterator = iterator->next;
	}
	return -1; //not found
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	if(queue == NULL || func == NULL ) {
		fprintf(stderr, "iterate NULL parameter error");
		return -1;
	}


	node* iterator = queue->head;
	int retval = 0;
	int stop = 0;
	while(iterator != NULL && stop == 0) {

		retval = func(queue, iterator->data, arg);
		if(retval == 1) {
			if(data != NULL) {

				*data = iterator->data;
				
			}
			stop = 1;
			iterator = NULL;

		}
		else {
		iterator = iterator->next;
		}
	}

	//???free(iterator);
	return 0;
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	//are we allowed to have a length in queue? it would makes sense I think, also allows O(1) time.
	return queue->length;
	/* TODO Phase 1 */
}

int queue_enqueue(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL) {
		fprintf(stderr, "enqueue NULL parameter error");
		return -1;
	}

	struct node* new_element = NULL;
	new_element = (struct node*) malloc(sizeof(node));
	if(new_element == NULL) {
		fprintf(stderr, "malloc error, memory not available");
		return -1;
	}

	new_element->data = data;
	new_element->next = NULL;
	//to enqueue in O(1), size doesn't matter. needs to go to end of tail
	//tail node points to new end, then tail pointer points to new end (because tail is always at the end)

	//returns true if length != 0 right?
	if (queue->length) {
		//struct node* temp = queue->tail;
		//temp->next = new_element;
		//queue->tail = temp;
		queue->tail->next = new_element;
	} else {
		queue->head = new_element; //empty, head is only element
	}
	queue->tail = new_element;
	queue->length++;



	return 0;
	/* TODO Phase 1 */
}

int queue_dequeue(queue_t queue, void **data)
{
	//does NULL = false already? like if queue == NULL then if(!queue) means if(queue == NULL)?
	if(queue == NULL || queue->length == 0 || data == NULL ) {
		fprintf(stderr, "dequeue NULL or empty parameter error");
		return -1;
	}

	//**can you reduce this? probably not


	//if two element list, popping one, tail becomes NULL (tail shouldn't point to head as well even if only one element)
	if(queue->head->next == NULL) {
		queue->tail = NULL;
	}
	data = &queue->head->data;
	queue->head = queue->head->next;
	queue->length--;

	//data = *d or &d or just d? well you do int *a; a = &b so assuming its the same-ish
 
	//free(ele) or free((void*)ele)? should be void* cause you're freeing what pop_element was holding not it
	//also free(ele) though? or maybe does free(ele) free both?
	//hello
		

	return 0;

	
	/* TODO Phase 1 */
}

static int inc_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (intptr_t)arg;

    *a += inc;

    return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(queue_t q, void *data, void *arg)
{
    int *a = (int*)data;
    int match = (intptr_t)arg;

 
    if (*a == match)
        return 1;

    return 0;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    int *ptr;

    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < 10; i++)
        queue_enqueue(q, &data[i]);



    /* Add value '1' to every item of the queue */
    queue_iterate(q, inc_item, (void*)1, NULL);
 printf("The value of p is: %d\n", data[0]);    //assert(data[0] == 2);

    /* Find and get the item which is equal to value '5' */
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);
 printf("The value of p is: %d\n", *ptr);    //assert(*ptr == 5);
}

/* Size of the stack for a thread (in bytes) */
#define UTHREAD_STACK_SIZE 32768

void uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next)
{
	/*
	 * swapcontext() saves the current context in structure pointer by @prev
	 * and actives the context pointed by @next
	 */


	if (swapcontext(prev, next)) {
		perror("swapcontext");
		exit(1);
	}
}

void *uthread_ctx_alloc_stack(void)
{
	return malloc(UTHREAD_STACK_SIZE);
}

void uthread_ctx_destroy_stack(void *top_of_stack)
{
	free(top_of_stack);
}

/*
 * uthread_ctx_bootstrap - Thread context bootstrap function
 * @func: Function to be executed by the new thread
 * @arg: Argument to be passed to the thread
 */
static void uthread_ctx_bootstrap(uthread_func_t func, void *arg)
{
	/*
	 * Enable interrupts right after being elected to run for the first time
	 */
	printf("The value of p is: %d\n", 5);   

	preempt_enable();
	printf("The value of p is: %d\n", 4);   

	/* Execute thread and when done, exit with the return value */
	uthread_exit(func(arg));
}

int uthread_ctx_init(uthread_ctx_t *uctx, void *top_of_stack,
		     uthread_func_t func, void *arg)
{
	/*
	 * Initialize the passed context @uctx to the currently active context
	 */
	if (getcontext(uctx))
		return -1;

	/*
	 * Change context @uctx's stack to the specified stack
	 */
	uctx->uc_stack.ss_sp = top_of_stack;
	uctx->uc_stack.ss_size = UTHREAD_STACK_SIZE;

	/*
	 * Finish setting up context @uctx:
	 * - the context will jump to function uthread_ctx_bootstrap() when
	 *   scheduled for the first time
	 * - when called, function uthread_ctx_bootstrap() will receive two
	 *   arguments: @func and @arg
	 */

	makecontext(uctx, (void (*)(void)) uthread_ctx_bootstrap,
		    2, func, arg);

	return 0;
}




queue_t schedulerReady, schedulerZombie, schedulerRunning, schedulerBlocked;
uthread_t CurrentID = 1;

typedef enum {
	READY,
	ZOMBIE,
	RUNNING,
	BLOCKED
} thread_status;

/* TODO Phase 2 */
//Thread Control Block
typedef struct TCB {
	uthread_ctx_t *context;
	thread_status status;
	uthread_t TID;
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
		//return -1; void return, can't return -1 int
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

	queue_enqueue(schedulerZombie, temp);
 
	uthread_yield();
 
 
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

	/* TODO Phase 3 
	//if tid doesnt exists, then return error
	//tid == NULL can't happen cause tid is an int
	if(tid < 0 || uthread_self() == tid || tid > CurrentID)
		return -1;
	int *i;
	do
	{	//searches zombie queue for tid, if its inside of it, then join values from zombie
		//this is unfinished
		//void* tid? maybe context? why ints
		queue_iterate(schedulerZombie, find_item, (void*)tid, (void**)&i);
		if(*i == 1)
		{

		}

	}while(*i == 0);*/

}

int hello(void* arg)
{
	printf("Hello world!\n");
	return 0;
}

int main(void)
{
	uthread_t tid;
	
	tid = uthread_create(hello, NULL);
			 printf("The value of p is: %d\n", 2);   

	uthread_join(tid, NULL);

	return 0;
}
