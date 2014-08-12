/*
 * timer.h
 *
 *  Created on: Aug 8, 2014
 *      Author: nikhilvs9999
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <signal.h>



typedef void (*timer_callback)(int sig, siginfo_t *si, void *uc);
void initialize_timer(sigset_t * mask,timer_t * timerid,struct itimerspec * its,timer_callback);
void start_timer(int expiery_secs,int interval_secs,sigset_t *mask,timer_t timerid,struct itimerspec *its);
void stop_timer(sigset_t *mask,timer_t timerid,struct itimerspec *its);
void delete_timer(timer_t timerid);



#endif /* TIMER_H_ */
