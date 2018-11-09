#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sem.h>
#include <tps.h>

char msg1[20] = "Meow Meow!\n";
char msg2[20] = "Woof Woof!\n";
char msg3[20] = "=^.^=\n";

sem_t sem1, sem2;

void *thread2(void *arg)
{
    char *buffer = malloc(TPS_SIZE);

    tps_create();

    assert(tps_write(0, TPS_SIZE + 1, buffer) == -1); // lengthy
    assert(tps_write(1, TPS_SIZE, buffer) == -1);     // lengthy

    assert(tps_write(50, 20, msg2) == 0);

    sem_up(sem1);
    sem_down(sem2);

    // check if thread 2's page hasn't been changed to msg3 (it should be msg2)
    memset(buffer, 0, TPS_SIZE);
    assert(tps_read(50, 20, buffer) == 0);
    assert(!memcmp(msg2, buffer, 20));

    assert(tps_destroy() == 0);

    sem_up(sem1);

    free(buffer);

    return NULL;
}

void *thread1(void *arg)
{
    char *buffer = malloc(TPS_SIZE);
    char *buffer2 = NULL;

    // cant write/read if TPS not created
    assert(tps_read(-1, TPS_SIZE, buffer) == -1);
    assert(tps_write(-1, TPS_SIZE, buffer) == -1);

    assert(tps_destroy() == -1);

    // create TPS and initialize with *msg1
    tps_create();

    // cant have a null buffer
    assert(tps_read(-1, TPS_SIZE, buffer2) == -1);
    assert(tps_write(-1, TPS_SIZE, buffer2) == -1);

    // test if msg is written successfully even if offset > 0
    assert(tps_write(50, 20, msg1) == 0);

    // read from TPS and make sure it contains the message
    memset(buffer, 0, TPS_SIZE);
    assert(tps_read(50, 20, buffer) == 0);
    assert(!memcmp(msg1, buffer, 20));

    // corner cases that should cause the TPS API functions to return -1
    assert(tps_create() == -1); // thread's TPS already initialized
    assert(tps_read(-1, TPS_SIZE, buffer) == -1);  // cant have negative offsets
    assert(tps_write(-1, TPS_SIZE, buffer) == -1); // cant have negative offsets
    assert(tps_write(TPS_SIZE + 1, TPS_SIZE, buffer) == -1); // offset too large
    assert(tps_read(TPS_SIZE + 1, TPS_SIZE, buffer) == -1);  // offset too large

    assert(tps_destroy() == 0);

    // create a new thread2 and try cloning it to test tps_clone
    pthread_t tid;
    pthread_create(&tid, NULL, thread2, NULL);
    sem_down(sem1);

    tps_clone(tid);

    // check if copy & original points to the same page
    memset(buffer, 0, 20);
    tps_read(50, 20, buffer);
    assert(!memcmp(msg2, buffer, 20)); // clone worked

    // sem_up(sem2); the code before
    // sem_down(sem1); want to change this to have the tps 
    // write change the data, then check if that also changed 
    // thread 2's tps 

    // change page content and see if a new copy is made (page counter > 1)
    // thread 2's page content should not be changed
    assert(tps_write(50, 20, msg3) == 0);

    sem_up(sem2);
    sem_down(sem1);

    pthread_join(tid, NULL);

    free(buffer);

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t tid;

    /* create two semaphores for thread synchro */
    sem1 = sem_create(0);
    sem2 = sem_create(0);

    // init TPS API
    tps_init(1);

    // create thread 1 and wait
    pthread_create(&tid, NULL, thread1, NULL);
    pthread_join(tid, NULL);

    assert(tps_init(1) == -1); // TPS was already initialized
    printf("got here");

    // destroy resources and quit
    sem_destroy(sem1);
    sem_destroy(sem2);

    return 0;
}
