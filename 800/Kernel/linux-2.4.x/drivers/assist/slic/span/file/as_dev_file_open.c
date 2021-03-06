/*
 * $Author: lizhijie $
 * $Log: as_dev_file_open.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.6  2006/04/25 05:37:16  wangwei
 * no message
 *
 * Revision 1.5  2006/03/11 10:03:45  lizhijie
 * add DP in FXS and FLASH event rx operations
 *
 * Revision 1.4  2006/01/17 03:19:42  lizhijie
 * a
 *
 * Revision 1.3  2006/01/16 11:00:02  lizhijie
 * add soft gain and pulse dialing conditional compile
 *
 * Revision 1.2  2006/01/16 06:42:55  lizhijie
 * dd
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 *
 * Revision 1.2  2005/07/07 02:48:23  wangwei
 * no message
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.4  2004/12/17 07:36:51  lizhijie
 * add debug for IXP422, such as Hardware gain, DMA buffer size, software Gain
 *
 * Revision 1.3  2004/11/29 01:56:52  lizhijie
 * some little bug
 *
 * Revision 1.2  2004/11/22 01:54:04  lizhijie
 * add some user module into CVS
 *
 * Revision 1.1.1.1  2004/11/18 07:00:04  lizhijie
 * driver for assist telephone cards Tiger320-Si3210/3050
 *
 * $Revision: 1.1.1.1 $
*/
#include "asstel.h"
/*
 *  implement the file operation of open and release(close) for i-node
*/
spinlock_t bigzaplock = SPIN_LOCK_UNLOCKED;

static int __as_initialize_channel(struct as_dev_chan *chan)
{
	int res;
	unsigned long flags;
#if  AS_SOFT_GAIN
	void *rxgain=NULL;
#endif

#if AS_SOFT_ECHO
	echo_can_state_t *ec=NULL;
#endif

	if ((res = as_channel_reallocbufs(chan, AS_DEFAULT_BLOCKSIZE, AS_DEFAULT_NUM_BUFS)))
		return res;

	spin_lock_irqsave(&chan->lock, flags);

	chan->rxbufpolicy = AS_POLICY_IMMEDIATE;
	chan->txbufpolicy = AS_POLICY_IMMEDIATE;

#if AS_SOFT_ECHO
	/* Free up the echo canceller if there is one */
	ec = chan->ec;
	chan->ec = NULL;
	chan->echocancel = 0;
	chan->echostate = ECHO_STATE_IDLE;
	chan->echolastupdate = 0;
	chan->echotimer = 0;
#endif

	chan->txdisable = 0;
	chan->rxdisable = 0;

	//chan->digitmode = AS_DIGIT_MODE_DTMF;
	chan->digitmode = AS_DIGIT_MODE_NOTHING;
	chan->dialing = 0;
	chan->afterdialingtimer = 0;

	chan->cadencepos = 0;
	
	/* Timings for RBS */
#if 0	
	chan->prewinktime = ZT_DEFAULT_PREWINKTIME;
	chan->preflashtime = ZT_DEFAULT_PREFLASHTIME;
	chan->winktime = ZT_DEFAULT_WINKTIME;
	chan->flashtime = ZT_DEFAULT_FLASHTIME;

	if (chan->sig & __AS_SIG_FXO)
		chan->starttime = ZT_DEFAULT_RINGTIME;
	else
		chan->starttime = _DEFAULT_STARTTIME;
	
	chan->rxwinktime = ZT_DEFAULT_RXWINKTIME;
	chan->rxflashtime = ZT_DEFAULT_RXFLASHTIME;
	chan->debouncetime = ZT_DEFAULT_DEBOUNCETIME;
	
#endif
	chan->rxwinktime = AS_DEFAULT_RXWINKTIME;
	chan->rxflashtime = AS_DEFAULT_RXFLASHTIME;


	if (chan->sig & __AS_SIG_FXO)/* FXS device */
		chan->starttime = AS_RING_DEBOUNCE_TIME;
	else
		chan->starttime = AS_RING_DEBOUNCE_TIME;
	chan->debouncetime = AS_RING_DEBOUNCE_TIME;

	/* Initialize RBS timers */
	chan->itimerset = chan->itimer = chan->otimer = 0;
	chan->ringdebtimer = 0;		

	init_waitqueue_head(&chan->sel);
	init_waitqueue_head(&chan->readbufq);
	init_waitqueue_head(&chan->writebufq);
	init_waitqueue_head(&chan->eventbufq);
	init_waitqueue_head(&chan->txstateq);

	chan->gotgs = 0;
	chan->curtone = NULL;
	chan->tonep = 0;
//	set_tone_zone(chan, -1);

#if  AS_SOFT_GAIN
	if (chan->gainalloc && chan->rxgain)
		rxgain = chan->rxgain;
	as_channel_set_default_gain(chan);
#endif

	chan->eventinidx = chan->eventoutidx = 0;
	as_channel_set_law(chan,0);
//	zt_hangup(chan);


	chan->flags &= ~ AS_CHAN_FLAG_LINEAR;
//	if (chan->curzone) 
	{
		/* Take cadence from tone zone */
//		memcpy(chan->ringcadence, chan->curzone->ringcadence, sizeof(chan->ringcadence));
	} 
//	else 
	{
		/* Do a default */
//		memset(chan->ringcadence, 0, sizeof(chan->ringcadence));
//		chan->ringcadence[0] = chan->starttime;
//		chan->ringcadence[1] = ZT_RINGOFFTIME;
	}
	spin_unlock_irqrestore(&chan->lock, flags);

#if  AS_SOFT_GAIN
	if (rxgain)
		kfree(rxgain);
#endif

#if AS_SOFT_ECHO
	if (ec)
		echo_can_free(ec);
#endif

	return 0;
}


/* set all fields in this channel as the defaults */
void as_close_channel(struct as_dev_chan *chan)
{
	unsigned long flags;
	//void *rxgain = NULL;

#if AS_SOFT_ECHO
	echo_can_state_t *ec = NULL;
#endif

	as_channel_reallocbufs(chan, 0, 0); 
	spin_lock_irqsave(&chan->lock, flags);
#if AS_SOFT_ECHO
	ec = chan->ec;
	chan->ec = NULL;
#endif

#if 0
	chan->curzone = NULL;
#endif	
	chan->curtone = NULL;
	chan->cadencepos = 0;
//	zt_hangup(chan); 
	chan->itimerset = chan->itimer = 0;


#if AS_PULSE_DIALING
	chan->pulsecount = 0;
	chan->pulsetimer = 1;
	chan->pulsecount_20pps = 0;
	chan->pulsetimer_20pps = 1;
	chan->pulsepauseTimer_20pps = 1;

#endif


	chan->ringdebtimer = 0;
	init_waitqueue_head(&chan->sel);
	init_waitqueue_head(&chan->readbufq);
	init_waitqueue_head(&chan->writebufq);
	init_waitqueue_head(&chan->eventbufq);
	init_waitqueue_head(&chan->txstateq);
	//chan->txdialbuf[0] = '\0';
	//chan->digitmode = AS_DIGIT_MODE_DTMF;
	//chan->dialing = 0;
	chan->afterdialingtimer = 0;
	  /* initialize IO MUX mask */
	chan->iomask = 0;
	
#if  AS_SOFT_GAIN
	if (chan->gainalloc && chan->rxgain)
		rxgain = chan->rxgain;

	as_channel_set_default_gain( chan);
#endif

	chan->eventinidx = chan->eventoutidx = 0;
	chan->flags &= ~(AS_CHAN_FLAG_LINEAR);


	as_channel_set_law(chan,0);
/*
	memset(chan->conflast, 0, sizeof(chan->conflast));
	memset(chan->conflast1, 0, sizeof(chan->conflast1));
	memset(chan->conflast2, 0, sizeof(chan->conflast2));
*/
	spin_unlock_irqrestore(&chan->lock, flags);

#if  AS_SOFT_GAIN
	if (rxgain)
		kfree(rxgain);
#endif

#if AS_SOFT_ECHO
	if (ec)
		echo_can_free(ec);
#endif

}

int as_file_open(struct inode *inode, struct file *file)
{
	int res = 0;
	int unit = UNIT(file);
	struct as_dev_chan *chan = as_span_get_channel(unit);
	int inc = 1;

	if (chan && chan->sig) 
	{
		/* Make sure we're not already open, a net device, or a slave device */
		if (chan->flags & AS_CHAN_FLAG_OPEN) 
			res = -EBUSY;
		else 
		{
			/* Assume everything is going to be okay */
			res = __as_initialize_channel( chan );
			if (chan->flags & AS_CHAN_FLAG_PSEUDO) 
				chan->flags |= AS_CHAN_FLAG_AUDIO;
			if (chan->span && chan->span->open)
				res = chan->span->open( chan );
			if (!res) 
			{
				chan->file = file;
				if (inc)
					MOD_INC_USE_COUNT;
				chan->flags |= AS_CHAN_FLAG_OPEN;
			} 
			else 
			{
				as_close_channel( chan );
			}
		}
	}
	else
		res = -ENXIO;
	
	return res;
}

int  as_file_release(struct inode *inode, struct file *file)
{
	int res=0;
	int unit = UNIT(file);
	struct as_dev_chan *chan;
	chan = as_span_get_channel(unit);
	
	if (chan) 
	{
		chan->flags &= ~ AS_CHAN_FLAG_OPEN;
		chan->file = NULL;
		as_close_channel(chan );
		if (chan->span && chan->span->close)
			res = chan->span->close(chan );
	} 
	else
		res = -ENXIO;

	MOD_DEC_USE_COUNT;

	return res;
}

