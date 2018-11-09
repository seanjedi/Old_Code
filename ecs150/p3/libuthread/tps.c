#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "queue.h"
#include "thread.h"
#include "tps.h"

/*
 * tps_queue is shared with all threads so every time we access it we must use
 * the enter/exit critical section helper functions
 */
struct queue *tps_queue = NULL;

/*
 * The Page struct represents a memory's page address
 * @page = the address of the memory page
 * counter = keeps track how many TPSes are sharing the same memory page
 */
struct Page {
    void *page;
    int counter;
};

/*
 * The TPS struct associates a page of memory to a thread
 * tid = thread's thread ID
 * @page_ptr = current page a TPS is pointing to
 */
struct TPS {
    pthread_t tid;
    struct Page *page_ptr;
};

/*
 * find_by_tid: Callback function that verifies if a TPS is a match
 *
 * @q: queue holding the tps
 * @tps: current thread being iterated
 * @target_tid: thread ID being searched for (if it's in the queue)
 * Return: 1 if tid was found in the queue, else it returns 0
 */
static int find_by_tid(queue_t q, void *tps, void *target_tid)
{
    struct TPS *a = (struct TPS *)tps;
    pthread_t *match = (pthread_t *)target_tid;

    if (a->tid == *match) {
        return 1;
    }

    return 0;
}

/*
 * tps_self: Returns address of the current thread's TPS if there is one
 *
 * Return: NULL if current thread doesn't have a TPS else, returns the address
 * of the TPS struct corresponding to the current thread
 */
struct TPS *tps_self()
{
    struct TPS *tps = NULL;

    // search if current thread's tid is in the tps_queue
    pthread_t self = pthread_self();
    queue_iterate(tps_queue, find_by_tid, (void *)&self, (void **)&tps);

    return tps;
}

/*
 * find_by_page: Callback function that verifies if TPS queue has a given page
 *
 * @q: queue holding the tps
 * @tps: if a thread with given page address is found, make tps point to it
 * @page_fault: page address we are looking for in the queue
 * Return: 1 if  page was found in the queue, else it returns 0
 */
static int find_by_page(queue_t q, void *tps, void *page_fault)
{
    struct TPS *a = (struct TPS *)tps;
    void *match = page_fault;

    if (a->page_ptr->page == match) {
        return 1;
    }

    return 0;
}

/*
 * segv_handler: checks if a segmentation fault was caused by a TPS protection
 * error
 * sig: signal number
 * @si: signal information
 * context: hold the current context of the thread that was interrupted (but
 * this variable is not used in this assignment)
 */
static void segv_handler(int sig, siginfo_t *si, void *context)
{
    /*
     * Get the address corresponding to the beginning of the page where the
     * fault occurred
     */
    void *p_fault = (void *)((uintptr_t)si->si_addr & ~(TPS_SIZE - 1));

    /*
     * Iterate through all the TPS areas and find if p_fault matches one of them
     */
    struct TPS *tps = NULL;
    pthread_t self = pthread_self();
    queue_iterate(tps_queue, find_by_page, (void *)p_fault, (void **)&tps);

    if (tps != NULL) {
        /* Printf the following error message */
        fprintf(stderr, "TPS protection error!\n");
    }

    /* In any case, restore the default signal handlers */
    signal(SIGSEGV, SIG_DFL);
    signal(SIGBUS, SIG_DFL);
    /* And transmit the signal again in order to cause the program to crash */
    raise(sig);
}

int tps_init(int segv)
{
    enter_critical_section();

    if (tps_queue != NULL) {
        return -1;
    }

    tps_queue = queue_create();

    if (tps_queue == NULL) {
        return -1;
    }

    exit_critical_section();

    if (segv) {
        struct sigaction sa;

        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = segv_handler;
        sigaction(SIGBUS, &sa, NULL);
        sigaction(SIGSEGV, &sa, NULL);
    }

    return 0;
}

int tps_create(void)
{
    enter_critical_section();

    struct TPS *tps = tps_self();

    // current thread already has a TPS so return -1
    if (tps != NULL) {
        exit_critical_section();
        return -1;
    }

    // "else" initialize a new TPS for the current thread
    tps = (struct TPS *)malloc(sizeof(struct TPS));

    if (tps == NULL) {
        exit_critical_section();
        return -1;
    }

    tps->page_ptr = (struct Page *)malloc(sizeof(struct Page));
    tps->page_ptr->counter = 0;
    tps->page_ptr->page =
        mmap(NULL, TPS_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    tps->page_ptr->counter = 1;

    if (tps->page_ptr == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    tps->tid = pthread_self();

    queue_enqueue(tps_queue, tps);

    exit_critical_section();

    return 0;
}

int tps_destroy(void)
{
    enter_critical_section();

    struct TPS *tps = tps_self();

    // thread not found in TPS queue
    if (tps == NULL) {
        exit_critical_section();
        return -1;
    }

    queue_delete(tps_queue, (void *)tps);

    tps->page_ptr->counter--;

    // clean up page if we are the last thread using it
    if (tps->page_ptr->counter == 0) {
        munmap(tps->page_ptr->page, TPS_SIZE); // free mapped memory
        free(tps->page_ptr);
    }
    free(tps);

    exit_critical_section();

    return 0;
}

int tps_read(size_t offset, size_t length, char *buffer)
{
    enter_critical_section();

    struct TPS *tps = tps_self();

    if (tps == NULL || buffer == NULL || (offset + length) > TPS_SIZE ||
        offset > TPS_SIZE) {
        exit_critical_section();
        return -1;
    }
    // make memory page temporarily readavle
    if (mprotect(tps->page_ptr->page, TPS_SIZE, PROT_READ) < 0) {
        perror("mprotect read");
        return -1;
    }

    // adjust page address to account for the offset given by user
    void *curr_offset = tps->page_ptr->page + offset;
    memcpy(buffer, curr_offset, length);

    // remove reading privilege
    if (mprotect(tps->page_ptr->page, TPS_SIZE, PROT_NONE) < 0) {
        perror("mprotect none");
        return -1;
    }

    exit_critical_section();

    return 0;
}

int tps_write(size_t offset, size_t length, char *buffer)
{
    enter_critical_section();

    struct TPS *tps = tps_self();

    if (tps == NULL || buffer == NULL || (offset + length) > TPS_SIZE ||
        offset > TPS_SIZE) {
        exit_critical_section();
        return -1;
    }

    if (tps->page_ptr->counter > 1) {
        struct Page *page_cpy = (struct Page *)malloc(sizeof(struct Page));
        page_cpy->counter = 0;

        page_cpy->page =
            mmap(NULL, TPS_SIZE, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        // Change read/write permissions so we can copy the original page
        mprotect(tps->page_ptr->page, TPS_SIZE, PROT_READ);
        mprotect(page_cpy->page, TPS_SIZE, PROT_WRITE);

        memcpy(page_cpy->page, tps->page_ptr->page, TPS_SIZE);

        mprotect(tps->page_ptr->page, TPS_SIZE, PROT_NONE);
        mprotect(page_cpy->page, TPS_SIZE, PROT_NONE);

        // decrement original page before we make tps point at copy and lose ref
        tps->page_ptr->counter--;

        tps->page_ptr = page_cpy;
        tps->page_ptr->counter++;
    }

    // Make memory page temporarily writable
    if (mprotect(tps->page_ptr->page, TPS_SIZE, PROT_WRITE) < 0) {
        perror("mprotect write");
        return -1;
    }

    // adjust page address to account for the offset given by user
    void *curr_offset = tps->page_ptr->page + offset;
    memcpy(curr_offset, buffer, length);

    // Remove writing privilege
    if (mprotect(tps->page_ptr->page, TPS_SIZE, PROT_NONE) < 0) {
        perror("mprotect none");
        return -1;
    }

    exit_critical_section();

    return 0;
}

int tps_clone(pthread_t tid)
{
    enter_critical_section();

    struct TPS *tps = tps_self();

    // current thread already has a TPS so return -1
    if (tps != NULL) {
        exit_critical_section();
        return -1;
    }

    struct TPS *target_tps = NULL;

    // target thread must have a TPS else return -1
    queue_iterate(tps_queue, find_by_tid, (void *)tid, (void **)&target_tps);

    if (target_tps == NULL) {
        exit_critical_section();
        return -1;
    }

    // create a TPS for the current thread
    tps = (struct TPS *)malloc(sizeof(struct TPS));

    if (tps == NULL) {
        exit_critical_section();
        return -1;
    }

    // target_tps has the page address we want to share
    tps->tid = pthread_self();
    // make running thread's TPS only point at the target_tps's page
    tps->page_ptr = target_tps->page_ptr;
    tps->page_ptr->counter++;

    queue_enqueue(tps_queue, tps);

    exit_critical_section();

    return 0;
}
