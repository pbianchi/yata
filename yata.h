#ifndef __YATA_H__
#define __YATA_H__

/* Min timer value is 10ms and max value 10 minutes */
#define YATA_MIN_TIMER_INTERVAL		10
#define YATA_MAX_TIMER_INTERVAL		600000

#define YATA_MSECS_PER_SEC		1000
#define YATA_NSECS_PER_MSEC		1000000

typedef void *yata_timer;

int yata_timerstart(unsigned int tms, int t_periodic,
    void *t_handler_func, void *t_handler_args, yata_timer *timer);

int yata_timerstop(yata_timer *timer);

#endif /* __YATA_H__*/

