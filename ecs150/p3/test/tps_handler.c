#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <tps.h>

void *latest_mmap_addr; // make the address returned by mmap accessible

void *__real_mmap(void *addr, size_t len, int prot, int flags, int fildes,
                  off_t off);
void *__wrap_mmap(void *addr, size_t len, int prot, int flags, int fildes,
                  off_t off)
{
    latest_mmap_addr = __real_mmap(addr, len, prot, flags, fildes, off);
    return latest_mmap_addr;
}

void *thread1(void *arg)
{
    char *tps_addr;

    tps_create();

    /* Get TPS page address as allocated via mmap() */
    tps_addr = latest_mmap_addr;

    /* Cause an intentional TPS protection error */
    tps_addr[0] = '\0';

    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t tid;

    /* Init TPS API */
    tps_init(1);

    /* Create thread 1 and wait */
    pthread_create(&tid, NULL, thread1, NULL);
    pthread_join(tid, NULL);

    return 0;
}
