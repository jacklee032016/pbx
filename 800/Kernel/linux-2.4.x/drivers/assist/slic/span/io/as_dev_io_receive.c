/*
 * $Author: lizhijie $
 * $Log: as_dev_io_receive.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.12  2006/04/25 05:37:25  wangwei
 * no message
 *
 * Revision 1.11  2006/04/19 12:33:50  lizhijie
 * add CAR support for both FXS/FXO and debugs
 *
 * Revision 1.10  2006/04/19 12:32:47  lizhijie
 * no message
 *
 * Revision 1.9  2006/03/12 11:09:32  lizhijie
 * debug DTMF caller ID and tone play after ISR timer is modified
 *
 * Revision 1.8  2006/03/11 10:03:45  lizhijie
 * add DP in FXS and FLASH event rx operations
 *
 * Revision 1.7  2006/01/18 13:01:17  lizhijie
 * dd
 *
 * Revision 1.6  2006/01/17 03:19:42  lizhijie
 * a
 *
 * Revision 1.5  2006/01/16 11:00:02  lizhijie
 * add soft gain and pulse dialing conditional compile
 *
 * Revision 1.4  2006/01/16 06:42:55  lizhijie
 * dd
 *
 * Revision 1.3  2006/01/14 11:05:35  lizhijie
 * dd
 *
 * Revision 1.2  2006/01/14 04:07:59  lizhijie
 * add conditional compile in integrated environments and wc structure optimization
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.4  2004/11/26 12:34:01  lizhijie
 * add multi-card support
 *
 * Revision 1.3  2004/11/25 07:49:24  lizhijie
 * remove some comments
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
#include "as_fxs_common.h"

static inline void __as_io_channel_in_rbs_itimer_expire(struct as_dev_chan *chan)
{
	/* the only way this could have gotten here, is if a channel
	    went off hook longer then the wink or flash detect timeout */
	/* Called with chan->lock held */
	switch(chan->sig)
	{
		case AS_SIG_FXOLS:  /* if FXO, its definitely on hook */
		case AS_SIG_FXOGS:
		case AS_SIG_FXOKS:
			as_channel_qevent_nolock(chan, AS_EVENT_ONHOOK);
			chan->gotgs = 0; 
			break;
		default:  /* otherwise, its definitely off hook */
			as_channel_qevent_nolock(chan, AS_EVENT_RINGOFFHOOK); 
			break;
	}
}

static void __as_io_channel_in_hooksig_pvt(struct as_dev_chan *chan, as_rxsig_t rxsig)
{
	/* State machines for receive hookstate transitions 
		called with chan->lock held */
	if ((chan->rxhooksig) == rxsig) 
		return;

	chan->rxhooksig = rxsig;
	
	switch(chan->sig) 
	{
/****************************************************************
 * below , process for FXO devices                                                              *
 ****************************************************************/	
/* Case 2 : When FXS signal is used in this channel: this is a FXO device  */
		case AS_SIG_FXSKS:  /* FXS Kewlstart */
		  /* ignore a bit poopy if loop not closed and stable */
			if (chan->txstate != AS_TXSTATE_OFFHOOK) 
				break;
		/* fall through intentionally */
/* Case 2 : When FXS signal is used in this channel: this is a FXO device  */
		case AS_SIG_FXSGS:  /* FXS Groundstart */
			if (rxsig == AS_RXSIG_ONHOOK) 
			{
				chan->ringdebtimer = AS_RING_DEBOUNCE_TIME;
				chan->ringtrailer = 0;
				if (chan->txstate != AS_TXSTATE_DEBOUNCE) 
				{
					chan->gotgs = 0;
					as_channel_qevent_nolock(chan, AS_EVENT_ONHOOK);
				}
			}
			else if(rxsig == AS_RXSIG_OFFHOOK )
			{/* for both off-hook action and ringer off , lizhijie, 2006.01.16 */
				as_channel_qevent_nolock(chan,AS_EVENT_RINGOFFHOOK); 
				chan->gotgs = 1;
				chan->itimerset = chan->itimer = 0;
			}
			/* following is added for polarity reversal by lizhijie, 2006.03.25 */
			else if(rxsig == AS_RXSIG_POLARITY )
			{
				as_channel_qevent_nolock(chan,AS_EVENT_POLARITY); 
				chan->gotgs = 1;
				chan->itimerset = chan->itimer = 0;
			}
			else if(rxsig == AS_RXSIG_START )
			{
				as_channel_qevent_nolock(chan,AS_EVENT_RINGERON);
				chan->gotgs = 1;
				chan->itimerset = chan->itimer = 0;
			}
			else if(rxsig == AS_RXSIG_CAR )
			{
				as_channel_qevent_nolock(chan,AS_EVENT_CAR); /* send CAR signal*/
				chan->gotgs = 1;
				chan->itimerset = chan->itimer = 0;
			}
			else if(rxsig == AS_RXSIG_RING )
			{
				as_channel_qevent_nolock(chan,AS_EVENT_RINGEROFF); /* send the second RINGER-ON event */
				chan->gotgs = 1;
				chan->itimerset = chan->itimer = 0;
			}
			break;
			
/************************************************************* 
 * Below are process for FXS devices 								  *
 *************************************************************/
/* Case 3 : When FXO signal is used in this channel: this is a FXS device  */
	   	case AS_SIG_FXOGS: /* FXO Groundstart */
			if (rxsig == AS_RXSIG_START) 
			{
			  /* if havent got gs, report it */
				if (!chan->gotgs) 
				{
					as_channel_qevent_nolock(chan, AS_EVENT_RINGOFFHOOK);
					chan->gotgs = 1;
				}
			}
		/* fall through intentionally */

/* Case 3 : When FXO signal is used in this channel: this is a FXS device  */
		case AS_SIG_FXOLS: /* FXO Loopstart */
		case AS_SIG_FXOKS: /* FXO Kewlstart */
			switch(rxsig) 
			{
				case AS_RXSIG_OFFHOOK: /* went off hook */
			  /* if asserting ring, stop it */
					if (chan->txstate == AS_TXSTATE_START) 
					{
						as_channel_rbs_sethook(chan,AS_TXSIG_OFFHOOK, AS_TXSTATE_AFTERSTART, AS_AFTERSTART_TIME);
					}
					else	/*added ,2006.01.16 stop ring after off-hook ,lizhijie*/
						chan->txstate = AS_TXSTATE_OFFHOOK;
					
					chan->kewlonhook = 0;
#if AS_SLIC_DEBUG
					//printk("Off hook on channel %d(chan postion %d), itimer = %d, gotgs = %d\n", chan->channo, chan->chanpos, chan->itimer, chan->gotgs);
#endif
					if (chan->itimer) /* if timer still running */
					{
#if AS_PULSE_DIALING
			    			int plen = chan->itimerset - chan->itimer;

						if((chan->pulsecount == 0)&&(chan->pulsecount_20pps== 0))
						{
							if(plen <= AS_MAXPULSETIME_20PPS)
							{
								chan->pulseflag = AS_20PPS;
								chan->pulsepauseTimer = 1;
								chan->pulsecount = 0;
								chan->pulsetimer = 1;
							}
							else if(plen <= AS_MAXPULSETIME)
							{
								chan->pulseflag = AS_10PPS;
								chan->pulsepauseTimer_20pps = 1;
								chan->pulsecount_20pps = 0;
								chan->pulsetimer_20pps = 1;
							}
							else if(plen <= AS_MAXFLASHTIME)
							{
								chan->pulseflag =AS_FLASH_EVENT;
							}
							else
							{
								chan->pulseflag = AS_NO_PPS;
							}
						}

						if(chan->pulseflag == AS_20PPS)
						{							
							chan->pulsepauseTimer_20pps = AS_PULSE_DEBOUNCE_TIME_20PPS ;
							chan->pulsecount_20pps++;
							
							if (chan->pulsecount_20pps == 1)
							{
								chan->pulsetimer_20pps = AS_PULSETIMEOUT_20PPS;
								as_channel_qevent_nolock(chan, AS_EVENT_PULSE_START); 
							}	
						}
						else if (chan->pulseflag == AS_10PPS)
			    			{
			    				
							chan->pulsepauseTimer = AS_PULSE_DEBOUNCE_TIME ;
							chan->pulsecount++;
							
							if (chan->pulsecount == 1)
							{
								chan->pulsetimer = AS_PULSETIMEOUT;
								as_channel_qevent_nolock(chan, AS_EVENT_PULSE_START); 
							}	
			    			} 
						else if(chan->pulseflag == AS_FLASH_EVENT)
						{
#endif							
							as_channel_qevent_nolock(chan,AS_EVENT_WINKFLASH); 
#if AS_SLIC_DEBUG
							printk("channel %d, FLASH event emitted\n",chan->channo);
#endif
						}
					} 
					else 
					{
				  /* if havent got GS detect */
						if (!chan->gotgs) 
						{
							as_channel_qevent_nolock(chan,AS_EVENT_RINGOFFHOOK); 
							chan->gotgs = 1;
							chan->itimerset = chan->itimer = 0;
						}
					}
					chan->itimerset = chan->itimer = 0;
					break;
		    		case AS_RXSIG_ONHOOK: /* went on hook */
					chan->gotgs = 0;
//					as_channel_hangup( chan );
					chan->txstate = AS_TXSTATE_ONHOOK;
					/* ON-HOOK event must be emitted by timer expired , lizhijie,2006.03.11 */
//					as_channel_qevent_nolock(chan,AS_EVENT_ONHOOK); 
					
			  		/* if not during offhook debounce time */
					if ((chan->txstate != AS_TXSTATE_DEBOUNCE) &&
			    			(chan->txstate != AS_TXSTATE_KEWL) && 
			    			(chan->txstate != AS_TXSTATE_AFTERKEWL)) 
			    		{
						chan->itimerset = chan->itimer = chan->rxflashtime * 8;
					}
					if (chan->txstate == AS_TXSTATE_KEWL)
						chan->kewlonhook = 1;

					/* must assigned value of itimer in order to emit ON-HOOK event when itimer expired ,lizhijie,2006.03.11*/
					chan->itimerset = chan->itimer = AS_DEFAULT_RXFLASHTIME;// chan->rxflashtime * 8;

#if AS_SLIC_DEBUG
					//printk("On hook on channel %d(postion %d), itimer = %d, itimerset = %d\n", chan->channo,chan->chanpos, chan->itimer, chan->itimerset);
#endif

					
					break;
		    		default:
					break;
			}
			
		default:
			break;
	}
}

/* this is a export symbol which is called by low layer driver to enqueue a received 
 * signal from low layer device , for example wcfxs call it in ISR to notify a AS_RXSIG_RING  */
void as_io_channel_hooksig(struct as_dev_chan *chan, as_rxsig_t rxsig)
{
	  /* skip if no change */
	unsigned long flags;
	spin_lock_irqsave(&chan->lock, flags);
	__as_io_channel_in_hooksig_pvt(chan,rxsig);
	spin_unlock_irqrestore(&chan->lock, flags);
}


static inline void __as_io_channel_in_putaudio_chunk(struct as_dev_chan *ms, unsigned char *rxb)
{
	/* We transmit data from our master channel */
	/* Called with ss->lock held */
	/* Linear version of received data */
#if AS_SOFT_ECHO
	short putlin[AS_CHUNKSIZE];
#endif

/*
silence the data after dialing immediately  
*/
	if (ms->dialing) 
		ms->afterdialingtimer = 50;
	else if (ms->afterdialingtimer) 
		ms->afterdialingtimer--;
	if (ms->afterdialingtimer && (!(ms->flags & AS_CHAN_FLAG_PSEUDO))) 
	{
		/* Be careful since memset is likely a macro */
		rxb[0] = AS_LIN2X(0, ms);
		memset(&rxb[1], rxb[0], AS_CHUNKSIZE - 1);  /* receive as silence if dialing */
	} 

#if  AS_SOFT_GAIN
	for (x=0;x<AS_CHUNKSIZE;x++) 
	{
		rxb[x] = ms->rxgain[rxb[x]];
	}
#endif

#if AS_SOFT_ECHO
	for (x=0;x<AS_CHUNKSIZE;x++) 
	{
		putlin[x] = AS_XLAW(rxb[x], ms);
	}
#endif


#if AS_SOFT_ECHO
	if (ms->ec) 
	{
		for (x=0;x<AS_CHUNKSIZE;x++) 
		{/* detect the tone which stop echo canceler */
			if (echo_can_disable_detector_update(&ms->rxecdis, putlin[x])) 
			{
				printk("Disabled echo canceller because of tone (rx) on channel %d\n", ms->channo);
				ms->echocancel = 0;
				ms->echostate = ECHO_STATE_IDLE;
				ms->echolastupdate = 0;
				ms->echotimer = 0;
				kfree(ms->ec);
				ms->ec = NULL;
				break;
			}
		}
	}

	if (!(ms->flags &  AS_CHAN_FLAG_PSEUDO)) 
	{
		memcpy(ms->putlin, putlin, AS_CHUNKSIZE * sizeof(short));
		memcpy(ms->putraw, rxb, AS_CHUNKSIZE);
	}
#endif
	
}


static inline void __as_io_channel_in_putbuf_chunk(struct as_dev_chan *ms, unsigned char *rxb)
{
	/* We transmit data from our master channel */
	/* Called with ss->lock held */
	/* Our receive buffer */
	unsigned char *buf;

	int oldbuf;
	int eof=0;
	int abort=0;
	int left;

	int bytes = AS_CHUNKSIZE;

	while(bytes) 
	{
		abort = 0;
		eof = 0;
		/* Next, figure out if we've got a buffer to receive into */
		if (ms->inreadbuf > -1) 
		{/* after device file is open and buffer is allocated, then inreadbuf =0, following operation can be executed */
			/* Read into the current buffer */
			buf = ms->readbuf[ms->inreadbuf];
			left = ms->blocksize - ms->readidx[ms->inreadbuf];
			if (left > bytes)
				left = bytes;
			memcpy(buf + ms->readidx[ms->inreadbuf], rxb, left);
			rxb += left;
			ms->readidx[ms->inreadbuf] += left;
			bytes -= left;
				/* End of frame is decided by block size of 'N' */
			eof = (ms->readidx[ms->inreadbuf] >= ms->blocksize);

#if 0//AS_DEBUG
				printk("channel %d RX \r\n", ms->channo );
#endif
			if (eof)  
			{/* Finished with this buffer, try another. so EOF of this block is check */
				oldbuf = ms->inreadbuf;
				ms->readn[ms->inreadbuf] = ms->readidx[ms->inreadbuf];
#if 0//AS_DEBUG
				printk("EOF, goto next buffer, len is %d in channels %d\r\n", ms->readn[ms->inreadbuf], ms->channo );
#endif
				{
					ms->inreadbuf = (ms->inreadbuf + 1) % ms->numbufs;
					if (ms->inreadbuf == ms->outreadbuf) 
					{
						/* Whoops, we're full, and have no where else
						to store into at the moment.  We'll drop it
						until there's a buffer available */
#if AS_DEBUG
						printk("Out of storage space\n");
#endif
						ms->inreadbuf = -1;
						/* Enable the receiver in case they've got POLICY_WHEN_FULL */
						ms->rxdisable = 0;
					}
					if (ms->outreadbuf < 0) 
					{ /* start out buffer if not already */
						ms->outreadbuf = oldbuf;
					}
/* In the very orignal driver, it was quite well known to me (Jim) that there
was a possibility that a channel sleeping on a receive block needed to
be potentially woken up EVERY time a buffer was filled, not just on the first
one, because if only done on the first one there is a slight timing potential
of missing the wakeup (between where it senses the (lack of) active condition
(with interrupts disabled) and where it does the sleep (interrupts enabled)
in the read or iomux call, etc). That is why the read and iomux calls start
with an infinite loop that gets broken out of upon an active condition,
otherwise keeps sleeping and looking. The part in this code got "optimized"
out in the later versions, and is put back now. */
					if (!ms->rxdisable) 
					{ /* if receiver enabled */
						/* Notify a blocked reader that there is data available
						to be read, unless we're waiting for it to be full */
#if 0//AS_DEBUG
						printk("Notifying reader data in block %d\n", oldbuf);
#endif
						wake_up_interruptible(&ms->readbufq);
						wake_up_interruptible(&ms->sel);
//						if (ms->iomask & ZT_IOMUX_READ)
//							wake_up_interruptible(&ms->eventbufq);
					}
				}
			}

			if (abort) 
			{
				/* Start over reading frame */
				ms->readidx[ms->inreadbuf] = 0;

//				if ((ms->flags & AS_CHAN_FLAG_OPEN) && !ss->span->alarms) 
						/* Notify the receiver... */
//					__qevent(ss->master, abort);
			}
		} 
		else /* No place to receive -- drop on the floor */
			break;
	}
}

static void __as_io_channel_in_receive_chunk(struct as_dev_chan *chan, unsigned char *buf)
{
	/* Receive chunk of audio -- called with chan->lock held */
	char waste[AS_CHUNKSIZE];

	if (!buf) 
	{
		memset(waste, AS_LIN2X(0, chan), sizeof(waste));
		buf = waste;
	}
	
	if ((chan->flags & AS_CHAN_FLAG_AUDIO)  ) 
	{/* gain and echo train process */
		__as_io_channel_in_putaudio_chunk(chan, buf);
	}
	/* put it into the upper layer buffer pool */
	__as_io_channel_in_putbuf_chunk(chan, buf);
}

int as_io_span_in_receive(void *fxs)
{
	unsigned long flags;
	struct as_dev_chan  *chan;
	int i;
	int x;
	/*added for multi-cards support , lizhijie 2004.11.25 */
	struct wcfxs *wc = (struct wcfxs *)fxs;

	for (i=0;i<wc->cards;i++)
	{
		x = wc->cardslot[i];
		chan = wc->chans[x];

		spin_lock_irqsave(&chan->lock, flags);
			
		/* Process a normal channel */
		__as_io_channel_in_receive_chunk(chan, chan->readchunk);
			
		if (chan->itimer) 
		{
			chan->itimer -= AS_CHUNKSIZE;
			if (chan->itimer <= 0) 
			{
				__as_io_channel_in_rbs_itimer_expire(chan );
			}
		}

			
		if ( chan->ringdebtimer)
		{
			/* decreased by AS_CHUNKSIZE;
			chan->ringdebtimer--;
			*/
			chan->ringdebtimer -= AS_CHUNKSIZE;
		}
			
		if (chan->sig & __AS_SIG_FXS) 
		{
			if (chan->rxhooksig == AS_RXSIG_RING)
				chan->ringtrailer = AS_RINGTRAILER;
			else if (chan->ringtrailer) 
			{
				chan->ringtrailer-= AS_CHUNKSIZE;
					/* See if RING trailer is expired */
				if (!chan->ringtrailer && !chan->ringdebtimer) 
					as_channel_qevent_nolock( chan, AS_EVENT_RINGOFFHOOK);
			}
		}
			
#if AS_PULSE_DIALING

		if(chan->pulseflag == AS_10PPS && chan->pulsepauseTimer > 0 && chan->txstate == AS_TXSTATE_OFFHOOK)
		{
			chan->pulsepauseTimer -= AS_CHUNKSIZE;
		}
		if(chan->pulseflag == AS_20PPS && chan->pulsepauseTimer_20pps > 0 && chan->txstate == AS_TXSTATE_OFFHOOK )
		{
			chan->pulsepauseTimer_20pps -=AS_CHUNKSIZE ;
		}

		if (( chan->pulsetimer)||(chan->pulsetimer_20pps))
		{
			if(chan->pulseflag == AS_10PPS && chan->pulsetimer )
			{
				chan->pulsetimer -= AS_CHUNKSIZE;
			}

			if(chan->pulseflag == AS_20PPS && chan->pulsetimer_20pps )
			{
				chan->pulsetimer_20pps -= AS_CHUNKSIZE;
			}
			
			if (
				((chan->pulsepauseTimer<=0 && chan->txstate == AS_TXSTATE_OFFHOOK) ||chan->pulsetimer <= 0 )
				||((chan->pulsepauseTimer_20pps<=0 && chan->txstate == AS_TXSTATE_OFFHOOK) || chan->pulsetimer_20pps <= 0)
			)
			{
				if (chan->pulsecount)
				{
					if (chan->pulsecount > 12) 
					{
						printk("Got pulse digit %d on %s. Press too quickly!\n", chan->pulsecount, chan->name);
					} 
					else if (chan->pulsecount > 11) 
					{
						as_channel_qevent_nolock(chan, AS_EVENT_PULSEDIGIT | '#');
					} 
					else if (chan->pulsecount > 10) 
					{
						as_channel_qevent_nolock(chan, AS_EVENT_PULSEDIGIT | '*');
					} 
					else if (chan->pulsecount > 9) 
					{
						as_channel_qevent_nolock(chan, AS_EVENT_PULSEDIGIT | '0');
					} 
					else 
					{
						as_channel_qevent_nolock( chan, AS_EVENT_PULSEDIGIT | ('0' + chan->pulsecount));
					}

#if AS_SLIC_DEBUG
					printk(KERN_ERR "-----------10PPS Pulse Digit '%d'\n", chan->pulsecount );
#endif
				}
				if (chan->pulsecount_20pps)
				{

					if (chan->pulsecount_20pps > 12) 
					{
						printk("Got pulse digit %d on %s. Press too quickly!\n", chan->pulsecount_20pps, chan->name);
					} 
					else if (chan->pulsecount_20pps > 11) 
					{
						as_channel_qevent_nolock(chan, AS_EVENT_PULSEDIGIT | '#');
					} 
					else if (chan->pulsecount_20pps > 10) 
					{
						as_channel_qevent_nolock(chan, AS_EVENT_PULSEDIGIT | '*');
					} 
					else if (chan->pulsecount_20pps > 9) 
					{
						as_channel_qevent_nolock(chan, AS_EVENT_PULSEDIGIT | '0');
					} 
					else 
					{
						as_channel_qevent_nolock( chan, AS_EVENT_PULSEDIGIT | ('0' + chan->pulsecount_20pps));
					}

#if AS_SLIC_DEBUG
					printk(KERN_ERR "-----------20PPS Pulse Digit '%d'\n", chan->pulsecount_20pps );
#endif
				}

				chan->pulsecount = 0;
				chan->pulsetimer = 1;
				chan->pulsepauseTimer = 1;

				chan->pulsecount_20pps = 0;
				chan->pulsetimer_20pps = 1;
				chan->pulsepauseTimer_20pps = 1;

				chan->pulseflag = AS_NO_PPS;
			}
		}
#endif

		spin_unlock_irqrestore(&chan->lock, flags);
#if 0
		chan = chan->next;
#endif
	}

	return 0;
}

