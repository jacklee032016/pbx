/*
* $Id: as_timer.h,v 1.1 2007/03/26 08:53:47 lizhijie Exp $
*/
#ifndef	__AS_TIMER_H__
#define	__AS_TIMER_H__

typedef	int (*as_timer_handle_t)(void *data);

int	as_timer_register_handler(as_timer_handle_t handler, void *data);
void as_timer_unregister_handler();
int	as_timer_start(int timeout/* seconds */);
int	as_timer_cancel();

#endif

