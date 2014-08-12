#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include "timer_api.h"

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

static void handler(int sig, siginfo_t *si, void *uc)
{
	/* Note: calling printf() from a signal handler is not
	 strictly correct, since printf() is not async-signal-safe;
	 see signal(7) */

	printf("Caught signal %d\n", sig);
//           print_siginfo(si);
//           signal(sig, SIG_IGN);
}

void initialize_timer(sigset_t * mask, timer_t * timerid,
		struct itimerspec * its, timer_callback callback_function)
{

	struct sigevent sev;
	struct sigaction sa;

	/* Establish handler for timer signal */

	printf("Establishing handler for signal %d\n", SIG);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = callback_function;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIG, &sa, NULL) == -1)
		errExit("sigaction");

	/* Block timer signal temporarily */

	printf("Blocking signal %d\n", SIG);
	sigemptyset(mask);
	sigaddset(mask, SIG);
	if (sigprocmask(SIG_SETMASK, mask, NULL) == -1)
		errExit("sigprocmask");

	/* Create the timer */

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIG;
	sev.sigev_value.sival_ptr = timerid;
	if (timer_create(CLOCKID, &sev, timerid) == -1)
		errExit("timer_create");

	printf(" timer ID is *timerid:0x%lx\n", (long) *timerid);

	/*Disarmed the timer by setting 0 */

	its->it_value.tv_sec = 0LL;
	its->it_value.tv_nsec = 0LL;
	its->it_interval.tv_sec = 0LL;
	its->it_interval.tv_nsec = 0LL;

	if (timer_settime(*timerid, 0, its, NULL) == -1)
		errExit("timer_settime");

}
void start_timer(int expiery_secs, int interval_secs, sigset_t *mask,
		timer_t timerid, struct itimerspec *its)
{

	/* Unlock the timer signal, so that timer notification
	 can be delivered */

	printf("\nStarting Timer sigid : %d\n", SIG);
	if (sigprocmask(SIG_UNBLOCK, mask, NULL) == -1)
		errExit("sigprocmask");

	/*arm the timer by setting proper values */

	its->it_value.tv_sec = (long long) expiery_secs;
	its->it_value.tv_nsec = 0LL;
	its->it_interval.tv_sec = (long long) interval_secs;
	its->it_interval.tv_nsec = 0LL;

	if (timer_settime(timerid, 0, its, NULL) == -1)
		errExit("timer_settime");

}

void stop_timer(sigset_t *mask, timer_t timerid, struct itimerspec *its)
{
	/* Block timer signal temporarily */

	printf("Stopping Timer sigid : %d\n", SIG);
	sigemptyset(mask);
	sigaddset(mask, SIG);
	if (sigprocmask(SIG_SETMASK, mask, NULL) == -1)
		errExit("sigprocmask");

	/* Disarmed the timer by setting 0 */

	its->it_value.tv_sec = 0LL;
	its->it_value.tv_nsec = 0LL;
	its->it_interval.tv_sec = 0LL;
	its->it_interval.tv_nsec = 0LL;

	if (timer_settime(timerid, 0, its, NULL) == -1)
		errExit("timer_settime");
}

void delete_timer(timer_t timerid)
{
	if (timer_delete(timerid) < 0)
	{
		errExit("timer_delete");
	}
}

/*int
 main(int argc, char *argv[])
 {

 sigset_t mask;
 timer_t timerid;
 struct itimerspec its;
 initialize_timer(&mask,&timerid,&its,handler);
 start_timer(2,1,&mask,timerid,&its);
 usleep(10*1000*1000);
 usleep(10*1000*1000);
 delete_timer(timerid);
 printf("exiting ....");
 exit(EXIT_SUCCESS);
 }*/
