/*
* $Id: as_timer.c,v 1.3 2007/03/27 09:53:14 lizhijie Exp $
*/

#include <signal.h>
#include <sys/time.h>
#include "aslib.h"

typedef	struct
{
	int				timeout;
	as_timer_handle_t	handler;
	void				*data;
}as_timer_t;


static as_timer_t	_as_timer =
{
	timeout	:	-1,
	handler	:	NULL,
	data		:	NULL
};

static void __timer_signal_handler(int signo)
{
	AS_MSG_LOG(AS_LOG_INFO, "Timer emitted");
	if (signo == SIGALRM)
	{
		if(_as_timer.handler)
		{
			(*_as_timer.handler)(_as_timer.data);
			as_timer_register_handler(_as_timer.handler, _as_timer.data);
		}
		else
		{
			AS_MSG_LOG(AS_LOG_ERR, "No Timer Handler is registered now");
		}
	}
}

int	as_timer_register_handler(as_timer_handle_t handler, void *data)
{
	_as_timer.handler = handler;
	_as_timer.data = data;

	struct sigaction act;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGALRM);
	act.sa_handler = __timer_signal_handler;
	sigaction(SIGALRM, &act, NULL);

	return 0;
}

void as_timer_unregister_handler()
{
	as_timer_cancel();
	_as_timer.handler = NULL;
	_as_timer.data = NULL;
}


int	as_timer_start(int timeout/* seconds */)
{
	_as_timer.timeout = timeout;
	return alarm(_as_timer.timeout); // set a timer.
}

int	as_timer_cancel()
{
	alarm(0);
	return 0;
}

