#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "thread.h"

struct semaphore {
    int count;
    struct queue *unblocked_threads;
    struct queue *blocked_threads;
};

sem_t sem_create(size_t count)
{
    struct semaphore *sem =
        (struct semaphore *)malloc(sizeof(struct semaphore));

    sem->count = count;
    sem->blocked_threads = queue_create();
    sem->unblocked_threads = queue_create();

    return sem;
}

int sem_destroy(sem_t sem)
{
    if (sem == NULL || queue_length(sem->unblocked_threads) > 0) {
        return -1;
    }
    queue_destroy(sem->blocked_threads);
    queue_destroy(sem->unblocked_threads);
    free(sem);

    return 0;
}

int sem_down(sem_t sem)
{
    if (sem == NULL) {
        return -1;
    }

    enter_critical_section();

    if (sem->count == 0) {
        queue_enqueue(sem->blocked_threads, (void *)pthread_self());
        thread_block();
    }
    sem->count--;

    exit_critical_section();

    return 0;
}

int sem_up(sem_t sem)
{
    if (sem == NULL) {
        return -1;
    }

    enter_critical_section();

    if (queue_length(sem->blocked_threads) != 0) {
        pthread_t temp = 0;
        queue_dequeue(sem->blocked_threads, (void **)&temp);

        thread_unblock(temp);
    }
    sem->count++;

    exit_critical_section();

    return 0;
}
