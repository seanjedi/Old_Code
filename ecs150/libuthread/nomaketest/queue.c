#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"


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

	struct node* pop_element = queue->head;
	//if two element list, popping one, tail becomes NULL (tail shouldn't point to head as well even if only one element)
	if(pop_element->next == NULL) {
		queue->tail = NULL;
	}
	queue->head = pop_element->next;
	queue->length--;

	//data = *d or &d or just d? well you do int *a; a = &b so assuming its the same-ish
	data = &(pop_element->data);
	//free(ele) or free((void*)ele)? should be void* cause you're freeing what pop_element was holding not it
	//also free(ele) though? or maybe does free(ele) free both?
	free(pop_element);
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
				queue->length--;

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

