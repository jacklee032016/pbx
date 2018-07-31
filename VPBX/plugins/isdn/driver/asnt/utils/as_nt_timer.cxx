/*
* $Id: as_nt_timer.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#if __ARM_IXP__
#include <arm/bitops.h>
#else
#include <asm/bitops.h>
#endif

#include "Sptr.hxx"
#include "cpLog.h"

#include "asnt_isdn.h"
#include "IsdnDeviceMgr.hxx"
#include "IsdnNtDevice.hxx"
#include "PbxTimerEvent.hxx"

#include "helper.h"

using namespace Assist;

static PbxTimerEvent *_asnt_timer_get(itimer_t *timer)
{
	return (PbxTimerEvent *)timer->queueTimer;
}


/* just init the data structure of timer */
int  asnt_timer_init(itimer_t *it, NTSTACK *nst)
{
	IsdnNtDevice			*ntCtrl;
	PbxTimerEvent 		*timerEvent = NULL;

	if (!nst)
		return(-ENODEV);
	
	ntCtrl = (IsdnNtDevice *)nst->priv;
	assert( ntCtrl != 0);
	
	if (!it)
		return(-ENODEV);
	
	if (test_bit(FLG_TIMER_RUNING, &it->Flags))
	{	
		cpLog(LOG_WARNING, "init timer(%x) while running", it->id);
		timerEvent = (PbxTimerEvent *)it->queueTimer;
//		if(timerEvent->getTimerId() != 0)
		if(timerEvent->getTimerId() > 0)
		{	
			timerEvent->cancelTimer();
			/* when cancelTimer, this object is destructed by sptr automatically */
			it->queueTimer = NULL;
		}
		else
		{/* timer has been add into FIFO */			
			delete timerEvent;
		}	
		test_and_clear_bit(FLG_TIMER_RUNING, &it->Flags);
	}
	
	it->Flags = 0;
	it->nst = nst;

	timerEvent = ntCtrl->createNtTimer();
	timerEvent->setPrivate( it);
	it->queueTimer = (void *)timerEvent;
	it->id = (int)it;//(int) timerEvent->getTimerId();

	cpLog(LOG_INFO, "init timer(%x) ", it->id);
	return 0;
}

/* added timer : start timer as the assigned expired time */
int asnt_timer_add(itimer_t *it)
{
	PbxTimerEvent *timerEvent ;

	timerEvent = _asnt_timer_get(it);
	if ( timerEvent ==0)
	{	
		cpLog(LOG_WARNING, "Timer not inited now");
		return(-ENODEV);
	}
	if (asnt_timer_pending(it))
		return(-EBUSY);
	
	cpLog(LOG_DEBUG, "add timer(%x)", it->id);

#if 0
	if(! test_and_set_bit(FLG_TIMER_RUNING, &it->Flags) )
	{
		cpLog(LOG_DEBUG, "timer(%x) is reinited when added.....", it->id);
		asnt_timer_init(it, it->nst );
		timerEvent = _asnt_timer_get( it);
	}
#endif

	test_and_set_bit(FLG_TIMER_RUNING, &it->Flags);
	timerEvent->startTimer(timerEvent, it->expires );

	cpLog(LOG_DEBUG,"add timer %p (%d ms)", it, it->expires);
	
	return 0;
}

/* delete timer : stop timer before expired
* not free, so this timer can be re-add (re-start)
*/
int asnt_timer_del(itimer_t *it)
{
	PbxTimerEvent *timerEvent = _asnt_timer_get(it);
	if ( timerEvent == NULL)
		return(-ENODEV);
	
	cpLog(LOG_DEBUG, "del timer(%x)", it->id);
	if(test_and_clear_bit(FLG_TIMER_RUNING, &it->Flags))
	{
		cpLog(LOG_WARNING, "timer(%x) is running");
		timerEvent->cancelTimer();
		/* when startTimer, a Sptr of PbxTimerEvent is pass into FIFO queue 
		* so cancelTimer, this Sptr is destructed */
		it->queueTimer = NULL;
		cpLog(LOG_DEBUG, "del timer(%x)", it->id);
	}
	else
	{
//		free(timerEvent );
//		it->queueTimer = NULL;
		cpLog(LOG_WARNING, "timer(%x) is not running when deleted");
	}
	cpLog(LOG_DEBUG, "del timer(%x)", it->id);
	return 0;
}

/* stop the timer and free data structure of timer */ 
int asnt_timer_remove(itimer_t *it)
{
	PbxTimerEvent *timerEvent = _asnt_timer_get( it);
	
	if (timerEvent== NULL)
		return(-ENODEV);

	cpLog(LOG_DEBUG, "remove timer(%x)", it->id);
	if( test_and_clear_bit(FLG_TIMER_RUNING, &it->Flags) )
	{
		cpLog(LOG_DEBUG, "remove timer(%x)", it->id);
		timerEvent->cancelTimer();
		cpLog(LOG_DEBUG, "remove timer(%x)", it->id);
	}
	else if( timerEvent->getTimerId() == 0)
	{
		free(timerEvent);
	}
	cpLog(LOG_DEBUG, "remove timer(%x)", it->id);
	
	it->queueTimer = NULL;
	
	cpLog(LOG_DEBUG, "remove timer(%x)", it->id);
	return 0;
}

int asnt_timer_pending(itimer_t *it)
{
	return(test_bit(FLG_TIMER_RUNING, &it->Flags));
}

int asnt_timer_handle(itimer_t *timer)
{
	int		ret = 0;

	if(timer==NULL)
	{
		return (-ENODEV);
	}
	/*the PbxTimerEvent is destructed when expired in a Sptr FIFO queue automatically */	
//	PbxTimerEvent *timerEvent = _asnt_timer_get( timer);
//	if (!timerEvent)
//		return(-ENODEV);
//	free(timerEvent);	
	cpLog(LOG_DEBUG, "handle timer(%x)", timer->id);

	timer->queueTimer = NULL;
	test_and_clear_bit(FLG_TIMER_RUNING, &timer->Flags);

	TRACE;
	if (timer->function)
		ret = timer->function( timer->data);

	TRACE;
	return  (ret);
}

