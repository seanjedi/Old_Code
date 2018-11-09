#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100



void preempt_disable(void)
{sigset_t x;
sigemptyset (&x);
sigaddset(&x, SIGVTALRM);
	/* TODO Phase 4 */
	sigprocmask(SIG_SETMASK, &x, NULL);
}

void preempt_enable(void)
{sigset_t x;
sigemptyset (&x);
sigaddset(&x, SIGVTALRM);
	/* TODO Phase 4 */
	sigprocmask(SIG_UNBLOCK, &x, NULL);
}

void handler(int signum){
	uthread_yield();
}


void preempt_start(void)
{
	/* TODO Phase 4 */
	struct itimerval alarm;
	
	struct sigaction signl;
	signl.sa_handler = handler;
	
	if(sigaction(SIGVTALRM, &signl, NULL) == -1)
		perror("SIGACTION");

	alarm.it_value.tv_sec = HZ; 
	alarm.it_value.tv_usec = 0; 
	alarm.it_interval.tv_sec = HZ;
	alarm.it_interval.tv_usec = 0;	
	

	setitimer (ITIMER_VIRTUAL, &alarm, NULL);



}



