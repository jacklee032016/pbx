/*
 * $Author: lizhijie $
 * $Log: as_dev_file_ctl.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.7  2006/04/19 12:33:50  lizhijie
 * add CAR support for both FXS/FXO and debugs
 *
 * Revision 1.6  2006/04/19 12:32:56  lizhijie
 * no message
 *
 * Revision 1.5  2006/03/12 11:09:32  lizhijie
 * debug DTMF caller ID and tone play after ISR timer is modified
 *
 * Revision 1.4  2006/01/18 13:01:17  lizhijie
 * dd
 *
 * Revision 1.3  2006/01/17 03:19:42  lizhijie
 * a
 *
 * Revision 1.2  2006/01/16 11:00:02  lizhijie
 * add soft gain and pulse dialing conditional compile
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 *
 * Revision 1.2  2005/07/07 02:48:56  wangwei
 * no message
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.1.1.5  2004/12/31 08:48:46  fengshikui
 * no message
 *
 * Revision 1.1.1.4  2004/12/31 08:38:04  fengshikui
 * no message
 *
 * Revision 1.1.1.3  2004/12/31 08:23:51  fengshikui
 * �޸İ�
 *
 * Revision 1.1.1.2  2004/12/31 08:00:09  fengshikui
 * �޸İ�
 *
 * Revision 1.1.1.1  2004/11/18 07:00:04  lizhijie
 * driver for assist telephone cards Tiger320-Si3210/3050
 *
 * $Revision: 1.1.1.1 $
*/
#include "asstel.h"

/*
 * return DTMF signal to user space, DTMF signal is a charactor
*/
static int __as_channel_get_dtmf_detect(struct as_dev_chan *chan, struct file *file, unsigned long data)
{
	unsigned long flags;

	spin_lock_irqsave(&chan->lock, flags);
	
	if(!chan->dtmf_detect.dtmf_incoming)
		put_user(-1, (int *)data);
	else
	{
		put_user(chan->dtmf_detect.dtmf_current , (int *)data);
		chan->dtmf_detect.dtmf_incoming = 0;
		chan->dtmf_detect.dtmf_current = -1;
	}
	
	spin_unlock_irqrestore(&chan->lock, flags);
	
	return 0;
}


/*all  for cmd of AS_CTL_HOOK*/
static int __as_channel_hook_ioctl(struct as_dev_chan *chan, struct file *file, unsigned long data)
{
	unsigned long flags;
	int ret;
	int j, rv;

	if (chan->flags & AS_CHAN_FLAG_CLEAR)
		return -EINVAL;

	if (!chan->span) 
		return(0);

	get_user(j,(int *)data);

	if (chan->span->flags & AS_FLAG_RBS) /* what i RBS signal ??? lzj*/
	{
		switch (j) 
		{
			case AS_ONHOOK:
				spin_lock_irqsave(&chan->lock, flags);
				as_channel_hangup(chan);
				spin_unlock_irqrestore(&chan->lock, flags);
				break;
			case AS_OFFHOOK:
				spin_lock_irqsave(&chan->lock, flags);
				if ((chan->txstate == AS_TXSTATE_KEWL) ||
				  (chan->txstate == AS_TXSTATE_AFTERKEWL)) 
				{
					spin_unlock_irqrestore(&chan->lock, flags);
					return -EBUSY;
				}
				/* change as following, 2006.01.16,lizhijie
				as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_DEBOUNCE, chan->debouncetime);
				*/
				if (chan->sig & __AS_SIG_FXO)/* FXS device */
					as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_OFFHOOK, 0);
				else
					/* no debounce is needed for FXO off-hook, so enter into OFFHOOk state directly, lizhijie, 2006.04.19
					as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_DEBOUNCE, chan->debouncetime);
					*/
					as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_OFFHOOK, 0);
				
				spin_unlock_irqrestore(&chan->lock, flags);
				break;
			case AS_RING:
			case AS_START:
				spin_lock_irqsave(&chan->lock, flags);
				if (chan->txstate != AS_TXSTATE_ONHOOK) 
				{
					spin_unlock_irqrestore(&chan->lock, flags);
					return -EBUSY;
				}
				if (chan->sig & __AS_SIG_FXO) 
				{/* FXS device */
					ret = 0;
					chan->cadencepos = 0;
					chan->cadenceType = AS_RING_TYPE_NORMAL;
					ret = chan->ringcadence[0];
					as_channel_rbs_sethook(chan, AS_TXSIG_START, AS_TXSTATE_RINGON, ret/AS_TIME_UNIT *AS_CHUNKSIZE);
				} 
				else		/* FXO device */
					as_channel_rbs_sethook(chan, AS_TXSIG_START, AS_TXSTATE_START, chan->starttime);

				spin_unlock_irqrestore(&chan->lock, flags);
				if (file->f_flags & O_NONBLOCK)
					return -EINPROGRESS;
				rv = 0; 
				return rv;
				break;

			case AS_RING_CAR:
				spin_lock_irqsave(&chan->lock, flags);
				if (chan->txstate != AS_TXSTATE_ONHOOK) 
				{
					spin_unlock_irqrestore(&chan->lock, flags);
					return -EBUSY;
				}
				if (chan->sig & __AS_SIG_FXO) 
				{/* FXS device */
					ret = 0;
					chan->cadencepos = 0;
					chan->cadenceType = AS_RING_TYPE_CAR;
					ret = chan->carcadence[0];
					as_channel_rbs_sethook(chan, AS_TXSIG_START, AS_TXSTATE_RINGON, ret/AS_TIME_UNIT *AS_CHUNKSIZE);
				} 
				/* not support FXO device */

				spin_unlock_irqrestore(&chan->lock, flags);
				if (file->f_flags & O_NONBLOCK)
					return -EINPROGRESS;
				rv = 0; 
				return rv;
				break;

			case AS_WINK:
				spin_lock_irqsave(&chan->lock, flags);
				if (chan->txstate != AS_TXSTATE_ONHOOK) 
				{
					spin_unlock_irqrestore(&chan->lock, flags);
					return -EBUSY;
				}
				as_channel_rbs_sethook(chan, AS_TXSIG_ONHOOK, AS_TXSTATE_PREWINK, chan->prewinktime);
				spin_unlock_irqrestore(&chan->lock, flags);
				if (file->f_flags & O_NONBLOCK)
					return -EINPROGRESS;
				
				rv = as_schedule_delay(&chan->txstateq);
				if (rv) return rv;
				break;
			case AS_FLASH:
				spin_lock_irqsave(&chan->lock, flags);
				if (chan->txstate != AS_TXSTATE_OFFHOOK) 
				{
					spin_unlock_irqrestore(&chan->lock, flags);
					return -EBUSY;
				}
				as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_PREFLASH, chan->preflashtime);
				spin_unlock_irqrestore(&chan->lock, flags);
				if (file->f_flags & O_NONBLOCK)
					return -EINPROGRESS;
				rv = as_schedule_delay(&chan->txstateq);
				if (rv) 
					return rv;
				break;
			case AS_RINGOFF:
				spin_lock_irqsave(&chan->lock, flags);
				as_channel_rbs_sethook(chan, AS_TXSIG_ONHOOK, AS_TXSTATE_ONHOOK, 0);
				spin_unlock_irqrestore(&chan->lock, flags);
				break;
			default:
				return -EINVAL;
			}
	} 
	else if (chan->span->sethook) 
	{
#if AS_SLIC_DEBUG
		printk("process HOOK ioctl with the function provided by SPAN\r\n");
#endif
		chan->span->sethook(chan, j);
	}	
	else
	{
		printk("process HOOK ioctl : not avalidate hook state\r\n");
		return -ENOSYS;
	}
	return 0;
}


int  as_file_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long data)
{
	int unit = UNIT(file);
	unsigned long flags;
	int j;
	
#if  AS_SOFT_GAIN
	void *rxgain=NULL;
#endif

	struct as_dev_chan *chan = as_span_get_channel(unit);

	if (!chan)
		return -ENOSYS;

	switch(cmd) 
	{
		case AS_CTL_AUDIOMODE:
		/* Only literal clear channels can be put in  */
//			if (chan->sig != ZT_SIG_CLEAR) 
//				return (-EINVAL);
			get_user(j, (int *)data);
			if (j) 
			{
				spin_lock_irqsave(&chan->lock, flags);
				chan->flags |= AS_CHAN_FLAG_AUDIO;
				spin_unlock_irqrestore(&chan->lock, flags);
			} 
			else 
			{
			/* Coming out of audio mode, also clear all 
			   conferencing and gain related info as well
			   as echo canceller */
				spin_lock_irqsave(&chan->lock, flags);
				chan->flags &= ~AS_CHAN_FLAG_AUDIO;
						
#if  AS_SOFT_GAIN
				if (chan->gainalloc && chan->rxgain)
					rxgain = chan->rxgain;
				else
					rxgain = NULL;

				as_channel_set_default_gain( chan);
				chan->gainalloc = 0;
				spin_unlock_irqrestore(&chan->lock, flags);

				if (rxgain)
					kfree(rxgain);
#endif

			}
			break;
		case AS_CTL_HOOK:
			return __as_channel_hook_ioctl( chan,  file, data);
			break;
		
		case AS_CTL_GET_DTMF_DETECT:
			return __as_channel_get_dtmf_detect(chan, file, data);
			break;
	      /* added by wuaiwu, 2006/01/11 */
		case AS_CTL_LOADZONE:
		case AS_GET_CHANNEL_NUMBER:
		case AS_GET_CHANNELS_STATES:
			return as_span_ioctl(inode, file,cmd,data);
		/* end of added */
		
		default:
			return as_chanandpseudo_ioctl(inode, file, cmd, data, unit);
	}
	return 0;
}

