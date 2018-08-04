/*
 * $Author: lizhijie $
 * $Log: as_dev_io_transmit.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.11  2006/04/25 05:37:32  wangwei
 * no message
 *
 * Revision 1.10  2006/04/19 12:33:50  lizhijie
 * add CAR support for both FXS/FXO and debugs
 *
 * Revision 1.9  2006/04/19 12:32:47  lizhijie
 * no message
 *
 * Revision 1.8  2006/03/12 11:09:32  lizhijie
 * debug DTMF caller ID and tone play after ISR timer is modified
 *
 * Revision 1.7  2006/03/11 10:03:45  lizhijie
 * add DP in FXS and FLASH event rx operations
 *
 * Revision 1.6  2006/01/18 13:01:17  lizhijie
 * dd
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
 * Revision 1.7  2004/12/31 09:39:09  fengshikui
 * move wakeup_interrupt to 238
 *
 * Revision 1.6  2004/12/24 12:16:15  lizhijie
 * Cdebug the process block and Fedback cuurent is offVS: ----------------------------------------------------------------------
 *
 * Revision 1.5  2004/11/29 01:57:39  lizhijie
 * some little bug
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

#define 	AS_DEBUG_TONE_TIMER				0



static void __as_fxo_send_dp(struct as_dev_chan *chan)
{

	if(chan->pulse_fxosend_single_count <= 0)
	{
	
		if(chan->pulsestoptime > 0)
		{
			chan->pulsestoptime -= AS_CHUNKSIZE;
		}
		else
		{
			if(chan->pulse_fxo_already_send_count >= chan->txdialbuf[0])
			{
					chan->dialing=0;
					chan->digitmode = AS_DIGIT_MODE_NOTHING;
					chan->pulse_fxo_already_send_count = 0;
					chan->pulse_fxosend_single_count = 0;
					chan->pulsestoptime =0;
					chan->pulsepauseTimer = 0;
					return ;
			}

			chan->pulse_fxosend_single_count = chan->txdialbuf[chan->pulse_fxo_already_send_count + 1];

			if(chan->digitmode == AS_DIGIT_MODE_20PPS)
				chan->pulsepauseTimer = AS_PULSE_DEBOUNCE_TIME_20PPS;
			else 
				chan->pulsepauseTimer = AS_PULSE_DEBOUNCE_TIME;

			chan->pulsestoptime = 0;
			chan->pulse_fxosend_hookstate = 0;

			chan->pulse_fxo_already_send_count++;
		}
	}
	
	if(chan->pulse_fxosend_single_count > 0)
	{
		chan->pulsepauseTimer -= AS_CHUNKSIZE;
		
		if(chan->pulsepauseTimer > 0)
		{
			
			if((chan->pulse_fxosend_hookstate == 0) &&
				(((chan->pulsepauseTimer <= AS_PULSE_DEBOUNCE_OFFHOOK_TIME_10PPS)&&(chan->digitmode == AS_DIGIT_MODE_10PPS))||
				((chan->pulsepauseTimer <= AS_PULSE_DEBOUNCE_OFFHOOK_TIME_20PPS)&&(chan->digitmode == AS_DIGIT_MODE_20PPS)))
			)
			{
				/*oNhook option*/
				as_chanannel_sethook(chan,AS_TXSIG_ONHOOK);
				chan->pulse_fxosend_hookstate =1;
			}
			
			if((chan->pulse_fxosend_hookstate == 1) &&
				(((chan->pulsepauseTimer <= AS_PULSE_DEBOUNCE_ONHOOK_TIME_10PPS)&&(chan->digitmode == AS_DIGIT_MODE_10PPS))||
				((chan->pulsepauseTimer <= AS_PULSE_DEBOUNCE_ONHOOK_TIME_20PPS)&&(chan->digitmode == AS_DIGIT_MODE_20PPS)))
			)
			{
				/*offhook option*/
				as_chanannel_sethook(chan,AS_TXSIG_OFFHOOK);
				chan->pulse_fxosend_hookstate = 2;
			}
		}
		else
		{
			chan->pulse_fxosend_hookstate = 0;
			
			if(chan->digitmode == AS_DIGIT_MODE_20PPS)
				chan->pulsepauseTimer = AS_PULSE_DEBOUNCE_TIME_20PPS;
			else 
				chan->pulsepauseTimer = AS_PULSE_DEBOUNCE_TIME;
			
			chan->pulse_fxosend_single_count --;
			if(chan->pulse_fxosend_single_count <= 0)
			{
				chan->pulse_fxosend_single_count = 0;
				if(chan->digitmode == AS_DIGIT_MODE_20PPS)
					chan->pulsestoptime = AS_PULSEMINSTOP_TIME_20PPS;
				else 
					chan->pulsestoptime = AS_PULSEMINSTOP_TIME_10PPS;
				chan->pulsepauseTimer = 0;
			}
		}
	}
	return;
	
}


void __as_io_channel_out_rbs_otimer_expire(struct as_dev_chan *chan)
{
	int len = 0;
	/* Called with chan->lock held */

/* otimer can be less than 0 */
	chan->otimer = 0;
	/* Move to the next timer state */	
	switch(chan->txstate) 
	{
		case AS_TXSTATE_RINGOFF:
		/* Turn on the ringer now that the silent time has passed */
			++chan->cadencepos;
			if (chan->cadencepos >= AS_MAX_CADENCE)
				chan->cadencepos = 0;
			
			if(chan->cadenceType == AS_RING_TYPE_NORMAL )
				len = chan->ringcadence[chan->cadencepos];
			else
				len = chan->carcadence[chan->cadencepos];

			if (!len) 
			{
				chan->cadencepos = 0;
//				len = chan->dsp->zone->data->ringcadence[chan->cadencepos];
				if(chan->cadenceType == AS_RING_TYPE_NORMAL )
					len = chan->ringcadence[chan->cadencepos];
				else
					len = chan->carcadence[chan->cadencepos];
			}
			as_channel_rbs_sethook(chan, AS_TXSIG_START, AS_TXSTATE_RINGON, len/AS_TIME_UNIT*AS_CHUNKSIZE);
			as_channel_qevent_nolock(chan, AS_EVENT_RINGERON);
/* added in 09.27*/
			wake_up_interruptible(&chan->txstateq);

			break;
		
		case AS_TXSTATE_RINGON:
		/* Turn off the ringer now that the loud time has passed */
			++chan->cadencepos;
			if (chan->cadencepos >= AS_MAX_CADENCE)
				chan->cadencepos = 0;
//			len = chan->ringcadence[chan->cadencepos];
			if(chan->cadenceType == AS_RING_TYPE_NORMAL )
				len = chan->ringcadence[chan->cadencepos];
			else
				len = chan->carcadence[chan->cadencepos];

			if (!len) 
			{
				chan->cadencepos = 0;
//				len = chan->dsp->zone->data->ringcadence[chan->cadencepos];
				if(chan->cadenceType == AS_RING_TYPE_NORMAL )
					len = chan->ringcadence[chan->cadencepos];
				else
					len = chan->carcadence[chan->cadencepos];
			}

		/* added by lizhijie 2004.11.15, for the callerID timer  */
			chan->firstDialingTimer = 300/AS_TIME_UNIT*AS_CHUNKSIZE;//chan->dsp->zone->data->pause;
			
			as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_RINGOFF, len/AS_TIME_UNIT*AS_CHUNKSIZE);
			as_channel_qevent_nolock(chan, AS_EVENT_RINGEROFF);
/* added in 09.27*/
			wake_up_interruptible(&chan->txstateq);

			break;
		
		case AS_TXSTATE_START:
		/* If we were starting, go off hook now ready to debounce */
			as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_AFTERSTART, AS_AFTERSTART_TIME);
			wake_up_interruptible(&chan->txstateq);
			break;
		
		case AS_TXSTATE_PREWINK:
		/* Actually wink */
			as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_WINK, chan->winktime);
			break;
		
		case AS_TXSTATE_WINK:
		/* Wink complete, go on hook and stabalize */
			as_channel_rbs_sethook(chan, AS_TXSIG_ONHOOK, AS_TXSTATE_ONHOOK, 0);
			if (chan->file && (chan->file->f_flags & O_NONBLOCK))
				as_channel_qevent_nolock(chan, AS_EVENT_HOOKCOMPLETE);
			wake_up_interruptible(&chan->txstateq);
			break;
		
		case AS_TXSTATE_PREFLASH:
		/* Actually flash */
			as_channel_rbs_sethook(chan, AS_TXSIG_ONHOOK, AS_TXSTATE_FLASH, chan->flashtime);
			break;

		case AS_TXSTATE_FLASH:
			as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_OFFHOOK, 0);
			if (chan->file && (chan->file->f_flags & O_NONBLOCK))
				as_channel_qevent_nolock(chan, AS_EVENT_HOOKCOMPLETE);
			wake_up_interruptible(&chan->txstateq);
			break;
	
		case AS_TXSTATE_DEBOUNCE:
			as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_OFFHOOK, 0);
		/* See if we've gone back on hook */
			if (chan->rxhooksig == AS_RXSIG_ONHOOK)
				chan->itimerset = chan->itimer = chan->rxflashtime * 8;
			wake_up_interruptible(&chan->txstateq);
			break;
		
		case AS_TXSTATE_AFTERSTART:
			as_channel_rbs_sethook(chan, AS_TXSIG_OFFHOOK, AS_TXSTATE_OFFHOOK, 0);
			if (chan->file && (chan->file->f_flags & O_NONBLOCK))
				as_channel_qevent_nolock(chan, AS_EVENT_HOOKCOMPLETE);
			wake_up_interruptible(&chan->txstateq);
			break;

		case AS_TXSTATE_KEWL:
			as_channel_rbs_sethook(chan, AS_TXSIG_ONHOOK, AS_TXSTATE_AFTERKEWL, AS_AFTERKEWLTIME);
			if (chan->file && (chan->file->f_flags & O_NONBLOCK))
				as_channel_qevent_nolock(chan, AS_EVENT_HOOKCOMPLETE);
			wake_up_interruptible(&chan->txstateq);
			break;

		case AS_TXSTATE_AFTERKEWL:
			if (chan->kewlonhook)  
			{
				as_channel_qevent_nolock(chan, AS_EVENT_ONHOOK);
			}
			chan->txstate = AS_TXSTATE_ONHOOK;
			chan->gotgs = 0;
			break;

		default:
			break;
	}
}

/* In order to echo cancel and gain amplify just before send out into line */
static inline void __as_io_channel_out_getaudio_chunk(struct as_dev_chan *ms, unsigned char *txb)
{
	/* Called with ss->lock held */
	/* Linear representation */
#if AS_SOFT_ECHO
	short getlin[AS_CHUNKSIZE];
	int x;
#endif

	/* Okay, now we've got something to transmit */
#if AS_SOFT_ECHO
	for (x=0;x<AS_CHUNKSIZE;x++)
		getlin[x] = AS_XLAW(txb[x], ms);
	
	if (ms->ec) 
	{
		for (x=0;x<AS_CHUNKSIZE;x++) 
		{
			/* Check for echo cancel disabling tone */
			if (echo_can_disable_detector_update(&ms->txecdis, getlin[x])) 
			{
				printk("Disabled echo canceller because of tone (tx) on channel %d\n", ms->channo);
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
#endif
	
#if 0
	if (ms->confmute || (ms->echostate & __ECHO_STATE_MUTE)) 
	{
		txb[0] = ZT_LIN2X(0, ms);
		memset(txb + 1, txb[0], ZT_CHUNKSIZE - 1);
		if (ms->echostate == ECHO_STATE_STARTTRAINING) 
		{
			/* Transmit impulse now */
			txb[0] = ZT_LIN2X(16384, ms);
			ms->echostate = ECHO_STATE_AWAITINGECHO;
		}
	}

	/* save value from last chunk */
	memcpy(ms->getlin_lastchunk, ms->getlin, AS_CHUNKSIZE * sizeof(short));
	/* save value from current */
	memcpy(ms->getlin, getlin, AS_CHUNKSIZE * sizeof(short));
	/* save value from current */
	memcpy(ms->getraw, txb, AS_CHUNKSIZE);
#endif

	/* if to make tx tone */
#if 0//AS_PROSLIC_DSP
//			txb[x] = AS_LIN2X(getlin[x], ms);
//#else
	if (ms->v1_1 || ms->v2_1 || ms->v3_1)
	{
		for (x=0;x<AS_CHUNKSIZE;x++)
		{
			getlin[x] += as_txtone_nextsample(ms);
			txb[x] = AS_LIN2X(getlin[x], ms);
		}
	}
#endif

#if  AS_SOFT_GAIN
	/* This is what to send (after having applied gain) */
	for (x=0;x<AS_CHUNKSIZE;x++)
		txb[x] = ms->txgain[txb[x]];
#endif

}

/* get data from channel and write it into txb buffer : channel memory DMA address */
static void __as_io_channel_out_getbuf_chunk(struct as_dev_chan *chan, unsigned char *txb)
{
	/* Called with ss->lock held */
	/* Buffer we're using */
	unsigned char *buf;
	/* Old buffer number */
	int oldbuf;
	/* Linear representation */
	/* How many bytes we need to process */
	int bytes = AS_CHUNKSIZE, left;
#if AS_PROSLIC_DSP
#else
	short  getlin;
	int i;
	unsigned char flags;
#endif	
	/* Let's pick something to transmit.  First source to
	   try is our write-out buffer.  Always check it first because
	   its our 'fast path' for whatever that's worth. */
	   
	   
	   /* move line 286   to here by fengshikui*/
	wake_up_interruptible(&chan->writebufq);
	   
	   
	while(bytes) 
	{
	      
		if (( chan->outwritebuf > -1) && !chan->txdisable) 
		{
		/* copy data from upper layer to the write chunk of channel for DMA*/
			buf= chan->writebuf[chan->outwritebuf];
			left = chan->writen[chan->outwritebuf] - chan->writeidx[chan->outwritebuf];
			if (left > bytes)
				left = bytes;

			memcpy(txb, buf + chan->writeidx[chan->outwritebuf], left);
			chan->writeidx[chan->outwritebuf]+=left;
			txb += left;
			bytes -= left;
#if 0//AS_DEBUG
			printk("Channel %d output data\r\n", chan->channo);
#endif
			/* Check buffer status */
			if ( chan->writeidx[chan->outwritebuf] >= chan->writen[chan->outwritebuf]) 
			{
				/* We've reached the end of our buffer.  Go to the next. */
				oldbuf = chan->outwritebuf;
				/* Clear out write index and such */
				chan->writeidx[oldbuf] = 0;
				chan->writen[oldbuf] = 0;
				chan->outwritebuf = (chan->outwritebuf + 1) % chan->numbufs; /*next buffer in circular */
				if (chan->outwritebuf == chan->inwritebuf) 
				{
					/* Whoopsies, we're run out of buffers.  Mark ours
					as -1 and wait for the filler to notify us that
					there is something to write */
					chan->outwritebuf = -1;
//					if (ms->iomask & (ZT_IOMUX_WRITE | ZT_IOMUX_WRITEEMPTY))
//						wake_up_interruptible(&ms->eventbufq);
					/* If we're only supposed to start when full, disable the transmitter */
					if (chan->txbufpolicy == AS_POLICY_WHEN_FULL)
						chan->txdisable = 1;
				}
				
				if (chan->inwritebuf < 0) 
				{
					/* The filler doesn't have a place to put data.  Now
					that we're done with this buffer, notify them. */
					chan->inwritebuf = oldbuf;
					
				}
/* this line is added by lizhijie for the case of write block ,09.27 
	wakeup the thread sleep in file write operation

   move this line here from brace to preventing from current process block
   lizhijie 2004.12.24
*/
				

			}
		}

		else if (chan->curtone ) 
		{
			/* in the state of RINGON, no tone data transfered, LiZhijie 2004.11.16 */
			if(chan->txstate==AS_TXSTATE_RINGON)
				return;
			if(chan->dialing )
			{/* timer for the callerID after first ring */
				if(chan->firstDialingTimer>0 )
				{
					chan->firstDialingTimer = chan->firstDialingTimer -AS_CHUNKSIZE;
#if AS_DEBUG_TONE_TIMER
					printk("return for dialingtimer\n");
#endif
					return;
				}
				else
				{
					chan->firstDialingTimer = 0;
				}
					
			}
				
#if AS_PROSLIC_DSP
			if(chan->tonep==0)
			{
#if AS_DEBUG_TONE_TIMER
				printk("output new tone to hardware\n");
#endif
			/* registers are only set here, get the lock of wcfxs  */
				(chan->slic_play_tone)(chan);
			}

			chan->tonep += AS_CHUNKSIZE;
			bytes -= AS_CHUNKSIZE;
#if AS_DEBUG_TONE_TIMER
			printk("output this tone continued tonep=%d  tonesample=%d\n", chan->tonep, chan->curtone->tonesamples);
#endif

#else
			left = chan->curtone->tonesamples - chan->tonep;
			if (left > bytes)
				left = bytes;
			for (i=0;i<left;i++) 
			{/* Pick our default value from the next sample of the current tone */
				getlin = as_tone_nextsample(&chan->ts, chan->curtone);
				*(txb++) = AS_LIN2X(getlin, chan);
			}
			chan->tonep+=left;
			bytes -= left;
#endif
			
			
			if ( chan->tonep >= chan->curtone->tonesamples) 
			{/* only the case that tonep equal tonesamples would be happened  
			 * when it happen, it indicate that this tone has ended  lzj 
			*/
				struct as_tone *last;
				/* Go to the next sample of the tone */
				chan->tonep = 0;
				last = chan->curtone;
				chan->curtone = chan->curtone->next;
				if (!chan->curtone) 
				{
					/* No more tones...  Is this dtmf or mf?  If so, go to the next digit */
					if (chan->dialing)
					{
#if AS_DEBUG_TONE_TIMER

						printk("out put another dtmf signal's tone\n");
#endif
						if(chan->dsp)
						{
#if 0
							(chan->dsp->play_dtmf)(chan->dsp, chan);
#endif
							(chan->dsp->play_caller_id)(chan->dsp, chan);
						}
					}	
				} 
				else 
				{
#if AS_PROSLIC_DSP	
#else
					if (last != chan->curtone && chan->dsp )
						(chan->dsp->init_tone_state)(chan->dsp, chan);
#endif					
				}
			}
		}

		else if (chan->flags & AS_CHAN_FLAG_CLEAR) 
		{
			/* Clear channels should idle with 0xff for the sake
			of silly PRI's that care about idle B channels */
			memset(txb, 0xff, bytes);
			bytes = 0;
		} 
		else 
		{/* Lastly we use silence on telephony channels */
			memset(txb, AS_LIN2X(0, chan), bytes);	
			bytes = 0;
		}
	}	
}


int as_io_span_out_transmit(void *fxs)
{
	unsigned long flags;
	struct as_dev_chan  *chan;
	struct as_dev_span *span;
	int x;
	int i;

	struct wcfxs *wc = (struct wcfxs *)fxs;
	span = wc->span;
	
/* every channel in this card (wcfxs) */
	for (i=0;i<wc->cards;i++)
	{
		x = wc->cardslot[i];
		chan = wc->chans[x];
		
		spin_lock_irqsave(&chan->lock, flags);

/* the master of this channel is refer to itself , eg. this channel is not a slave channel of others */
		if ( chan->otimer) 
		{
			chan->otimer -= AS_CHUNKSIZE;
			if ( chan->otimer <= 0) 
			{
				__as_io_channel_out_rbs_otimer_expire( chan );
			}
		}
			
		__as_io_channel_out_getbuf_chunk(chan, chan->writechunk);

#if AS_SOFT_ECHO|AS_SOFT_GAIN
		__as_io_channel_out_getaudio_chunk(chan, chan->writechunk);
#endif

/*add for FXO send DP , wangwei ,2006.04.21*/
		if(chan->dialing== 1&& (chan->digitmode == AS_DIGIT_MODE_10PPS ||chan->digitmode == AS_DIGIT_MODE_20PPS))
		{
			__as_fxo_send_dp(chan);
		}
	
		/*end of this channel is not a slave of others */
		spin_unlock_irqrestore(&chan->lock, flags);

	}

	return 0;
}

