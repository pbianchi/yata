#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#include <signal.h>
#include <time.h>

#include <syslog.h>

#include "yata.h"

/* Timer User data */
struct yata_data {
	void		*yd_handler_args;
	void		(*yd_handler_func)(void *);
	sem_t		yd_sem;
	int		yd_periodic;
	timer_t		yd_tid;
};

/*
 * The purpose of this funcion is to serve as a callback invoked by
 * the thread created when an armed timer expires. It will invoke the
 * handler function passed by argument to yata_timerstart().
 * Periodic timers execute indefinitely until yata_timerstop() is called
 * by the user application, and non periodic timers will be deleted after
 * the first expiration.
 */
static void
yata_timerthread(union sigval sv)
{
	struct yata_data *yd = (struct yata_data *)sv.sival_ptr;

	sem_wait(&yd->yd_sem);

	if (yd->yd_handler_func == NULL)
		goto timer_stop;

	(yd->yd_handler_func)(yd->yd_handler_args);

	if (yd->yd_periodic) {
		sem_post(&yd->yd_sem);
		return;
	}

timer_stop:
	sem_post(&yd->yd_sem);
	yata_timerstop((yata_timer)&yd);
}

/*
 * The purpsose of this function is delete an allocated timer and free
 * the memory allocated to the user data pointed by the yata_data structure.
 */
int
yata_timerstop(yata_timer *timer)
{
	struct yata_data *yd = (struct yata_data *)(*timer);

	sem_wait(&yd->yd_sem);

	if (timer_delete(yd->yd_tid) < 0) {
		sem_post(&yd->yd_sem);
		return (-1);
	}

	sem_destroy(&yd->yd_sem);

	free(yd->yd_handler_args);
	free(yd);

	return (0);
}

/*
 * The purpose of this funcion is to create a POSIX timer that can be
 * periodic or not. It receives as arguments the expiration time (ms),
 * a handler function to be executed when it expires, a pointer to user
 * arguments to be handled by the handler function and a fills a pointer
 * (void * timer) with the timer address. For non periodic timers, all the
 * allocated data by the user will be deleted after the expiration. For periodic
 * timers, the timer deletion can be done through yata_timerstop() giving to
 * it the tOsTimer pointer filled by yata_timerstart(). When calling
 * yata_timerstop() all the periodic timer data and user data will be
 * deallocated.
 */
int
yata_timerstart(unsigned int tms, int t_periodic,
    void *t_handler_func, void *t_handler_args, yata_timer *timer)
{
	struct sigevent      sev;
	struct itimerspec    its;
	long                 t_secinterval = 0;
	long                 t_nsecinterval = 0;
	long                 t_msecinterval;
	pthread_attr_t       pt_attr;
	struct yata_data    *yd;

	if (tms < YATA_MIN_TIMER_INTERVAL || tms > YATA_MAX_TIMER_INTERVAL)
		return (-1);

	memset(&sev, 0, sizeof(sev));
	memset(&its, 0, sizeof(its));

	if (pthread_attr_init(&pt_attr) != 0)
		return (-1);

	if (pthread_attr_setdetachstate(&pt_attr,
	    PTHREAD_CREATE_DETACHED) != 0)
		return (-1);

	/* Convert the specified interval in ms to seconds and nanoseconds */

	t_msecinterval = tms;

	t_secinterval = t_msecinterval / YATA_MSECS_PER_SEC;

	t_nsecinterval = (t_msecinterval - (t_secinterval * YATA_MSECS_PER_SEC)) * YATA_NSECS_PER_MSEC;

	if ((yd = malloc(sizeof(*yd))) == NULL)
		return (-1);

	if (sem_init(&yd->yd_sem, 0, 1) < 0)
		return (-1);

	yd->yd_handler_args = t_handler_args;
	yd->yd_handler_func = t_handler_func;
	yd->yd_periodic = t_periodic;

	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_value.sival_ptr = yd;
	sev.sigev_notify_function = yata_timerthread;
	sev.sigev_notify_attributes = &pt_attr;

	/* Initial expiration value */
	its.it_value.tv_sec = t_secinterval;
	its.it_value.tv_nsec = t_nsecinterval;

	/* Configure timer periodicity if applicable */
	if (t_periodic) {
		its.it_interval.tv_sec = t_secinterval;
		its.it_interval.tv_nsec = t_nsecinterval;
	}

	if (timer_create(CLOCK_MONOTONIC, &sev, &yd->yd_tid) < 0) {
		sem_destroy(&yd->yd_sem);
		free(yd);
		return (-1);
	}

	if (timer_settime(yd->yd_tid, 0, &its, NULL) < 0) {
		timer_delete(yd->yd_tid);
		sem_destroy(&yd->yd_sem);
		free(yd);
		return (-1);
	}

	*timer = yd;

	return (0);
}

