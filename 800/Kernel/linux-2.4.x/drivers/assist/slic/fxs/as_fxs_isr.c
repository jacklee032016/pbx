/*
 * $Author: lizhijie $
 * $Revision: 1.1.1.1 $
 * $Log: as_fxs_isr.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.10  2006/04/25 05:36:28  wangwei
 * no message
 *
 * Revision 1.9  2006/04/19 12:33:50  lizhijie
 * add CAR support for both FXS/FXO and debugs
 *
 * Revision 1.8  2006/04/19 12:31:48  lizhijie
 * no message
 *
 * Revision 1.7  2006/03/11 10:03:44  lizhijie
 * add DP in FXS and FLASH event rx operations
 *
 * Revision 1.6  2006/01/18 13:01:17  lizhijie
 * dd
 *
 * Revision 1.5  2006/01/17 12:10:35  lizhijie
 * dd
 *
 * Revision 1.4  2006/01/17 03:19:42  lizhijie
 * a
 *
 * Revision 1.3  2006/01/16 06:42:54  lizhijie
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
 * Revision 1.8  2004/12/17 07:36:51  lizhijie
 * add debug for IXP422, such as Hardware gain, DMA buffer size, software Gain
 *
 * Revision 1.7  2004/12/15 07:33:05  lizhijie
 * recommit
 *
 * Revision 1.6  2004/12/14 08:45:21  lizhijie
 * add ARM channel exchange
 *
 * Revision 1.5  2004/11/29 01:53:38  lizhijie
 * update some redudent code and register setting
 *
 * Revision 1.4  2004/11/26 12:33:31  lizhijie
 * add multi-card support
 *
 * Revision 1.3  2004/11/25 07:59:24  lizhijie
 * recommit all
 *
*/

#include "as_fxs_common.h"
/* WCFXS Interrupt Service Routine */
extern int debug ;
extern int robust ;

#define  AS_PULSE_DIALING			1

#define	FXO_CAR_BOUNCE_TIMER		(640*AS_CHUNKSIZE/AS_TIME_UNIT)
#define	FXO_RINGER_TIMEOUT		(4000*AS_CHUNKSIZE/AS_TIME_UNIT)

static inline void __wcfxs_voicedaa_check_hook(struct wcfxs *wc, int card)
{
	unsigned char res;
	signed char b;
	int poopy = 0;
	unsigned char reg;

	/* Try to track issues that plague slot one FXO's */
	b = as_fxs_getreg(wc, card, 5);
#if 1	
	if ((b & 0x2) || !(b & 0x8)) 
	{/* 0x8 : ON-HOOK Line Monitor */
		/* Not good -- don't look at anything else */
#if AS_SLIC_DEBUG
		if (debug)
			printk("Poopy (%02x) on card %d!\n", b, card + 1); 
#endif
		if(b==0)
		{
			as_fxs_setreg(wc, card, 1, 0x80);
			
			if ( (poopy = as_fxs_init_voicedaa(wc, card, 0, 0, 1))  )
			{
				printk("reset DAA failure :%d\n", poopy);
			}	
		}	
		poopy++;
	}
#endif	
	b &= 0x9b;
	if (wc->mod.fxo.offhook[card]) 
	{
		if (b != 0x9)
			as_fxs_setreg(wc, card, 5, 0x9);
	} 
	else 
	{
		if (b != 0x8)
			as_fxs_setreg(wc, card, 5, 0x8);
	}
//	if (poopy)
//		return;

	if (!wc->mod.fxo.offhook[card]) /* RING detect only in ON-HOOK state */
	{
		reg = as_fxs_getreg(wc, card, 4);
		res = reg&(FXO_REG4_RDTI);
		if(res)
		{
			reg = reg&(~(FXO_REG4_RDTI));
			as_fxs_setreg( wc, card, 4, reg);

			if (!wc->mod.fxo.wasringing[card]) 
			{
				wc->mod.fxo.wasringing[card] = 1;
				wc->mod.fxo.ringdebounce[card] = 0;
				as_io_channel_hooksig( wc->chans[card], AS_RXSIG_START);
#if AS_SLIC_DEBUG
				printk("Begin ringing...\n");
#endif
			}
			else
			{
#if AS_SLIC_DEBUG
				printk("ringdebounce =%d\n",wc->mod.fxo.ringdebounce[card] );
#endif
				if(wc->mod.fxo.ringdebounce[card] <= FXO_CAR_BOUNCE_TIMER)
				{
					as_io_channel_hooksig( wc->chans[card], AS_RXSIG_CAR);
#if AS_SLIC_DEBUG
					printk("CAR Signal on card %d of chip %s\n", card, wc->name);
#endif
				}
				else
				{
					as_io_channel_hooksig( wc->chans[card], AS_RXSIG_RING);
#if AS_SLIC_DEBUG
					printk("Ringer Off\n");
#endif
				}
				wc->mod.fxo.ringdebounce[card] = 0;
				wc->mod.fxo.wasringing[card] = 0;
			}
		}
		else
		{
			if (wc->mod.fxo.wasringing[card]) 
			{/* no ring is continue within 4 seconds, so timeout */
				wc->mod.fxo.ringdebounce[card] += (AS_CHUNKSIZE);
				if(wc->mod.fxo.ringdebounce[card]>=FXO_RINGER_TIMEOUT)
				{
					wc->mod.fxo.wasringing[card] = 0;
					wc->mod.fxo.ringdebounce[card] = 0;
				}
			}
#if 0
			else
			{
				wc->mod.fxo.ringdebounce[card] = 0;
			}
#endif			
		}

	}
#if 0
	{
		res =reg&0x40/* RDTN */ ; 
		if( res )
		{
			printk("Ring Polarity Negative\n");
			if(wc->mod.fxo.polarity[card]
		}
		res =reg&0x20/* RDTP */ ; 
		if( res )
		{
			printk("Ring Polarity Positive\n");
			if( (wc->mod.fxo.polarity[card]&0x20) != res )
			{
				
			}
		}
#if 0	
	if(wc->mod.fxo.polarity[card] != wc->mod.fxo.lastpol[card] )
	{
		printk("Ring Polarity reversal detected\n");
		wc->mod.fxo.lastpol[card] = wc->mod.fxo.polarity[card];
//		as_channel_qevent_nolock(wc->chans[card],AS_EVENT_POLARITY); 
		as_io_channel_hooksig(wc->chans[card],AS_RXSIG_POLARITY); 
	}
#endif
	}
#endif

#if 0
	/* no HOOK status check is needed, lizhijie,2006.04.17 */
	b = as_fxs_getreg(wc, card, 29);

	if (abs(b) < BATT_THRESH) 
	{
		wc->mod.fxo.nobatttimer[card]++;
#if 0
		if (wc->mod.fxo.battery[card])
			printk("Battery loss: %d (%d debounce)\n", b, wc->mod.fxo.battdebounce[card]);
#endif
		if (wc->mod.fxo.battery[card] && wc->mod.fxo.battdebounce[card]<= 0) 
		{/* battery timeout */
			if (debug)
				printk("NO BATTERY on %d!\n",  card + 1);
			wc->mod.fxo.battery[card] =  0;
			wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;

//			if ((!wc->mod.fxo.ohdebounce[card]) && wc->mod.fxo.offhook[card]) /* JATE */
			{/* Japan, add off-hook debounce, about 350 ms */
				as_io_channel_hooksig( wc->chans[card], AS_RXSIG_ONHOOK);
				if (debug)
					printk("Signalled On Hook on DAA\n");
#if	1//ZERO_BATT_RING
				wc->mod.fxo.onhook[card]++;
#endif
			}
		} 
		else if (!wc->mod.fxo.battery[card])
		{/* if last is no-battery, renewal the debounce */
			wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;
		}
		/* other cases, such as */
	} 
	
	else if (abs(b) > BATT_THRESH) 
	{/*voltage is bigger than threshold, it may be off-hook*/
		if (!wc->mod.fxo.battery[card] && wc->mod.fxo.battdebounce[card]<= 0) 
		{
#if AS_SLIC_DEBUG
			if (debug)
				printk("BATTERY on %d is %d(%d voltage)!\n",  card + 1, wc->mod.fxo.battery[card], b);
#endif

			if (wc->mod.fxo.onhook[card]) /* JATE support*/
			{
				wc->mod.fxo.onhook[card] = 0;
					printk("Signalled Off Hook on DAA\n");
				as_io_channel_hooksig( wc->chans[card], AS_RXSIG_OFFHOOK);
#if 1//AS_SLIC_DEBUG
//				if (debug)
					printk("Signalled Off Hook on DAA\n");
#endif
			}

			wc->mod.fxo.battery[card] = 1;
			wc->mod.fxo.nobatttimer[card] = 0;
			wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;
		}
		else if (wc->mod.fxo.battery[card])
		{/* if last status is battery, renewal the debounce */
			wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;
			wc->mod.fxo.ohdebounce[card] = OH_DEBOUNCE;
		}
		/* other cases, eg. no battery and battdebounce is bigger than 0, then just decrease the battdebounce */
	} 
	else
	{
		/* It's something else... */
		wc->mod.fxo.battdebounce[card] = BATT_DEBOUNCE;
	}
#endif


#if 0
	/* check POLI bit can not be used*/
	reg = as_fxs_getreg( wc, card, 4);
	res = (reg&0x01);
	if ( res )
	{
		printk("Bettery polarity reversal, cleaned(%d voltage)\n", b);
		as_fxs_setreg(wc, card, 4, reg&(~0x01) );
	}
#endif
	

	res = as_fxs_getreg(wc, card, 14);
	wc->mod.fxo.polarity[card] = res &0x02;
	if(wc->mod.fxo.polarity[card] != wc->mod.fxo.lastpol[card] )
	{
		printk("Ring Polarity reversal detected\n");
		wc->mod.fxo.lastpol[card] = wc->mod.fxo.polarity[card];
//		as_channel_qevent_nolock(wc->chans[card],AS_EVENT_POLARITY); 
		as_io_channel_hooksig(wc->chans[card],AS_RXSIG_POLARITY); 
	}
	
	if (wc->mod.fxo.battdebounce[card]>0)
		wc->mod.fxo.battdebounce[card] -= AS_CHUNKSIZE;
	
	if (wc->mod.fxo.ohdebounce[card]>0)
		wc->mod.fxo.ohdebounce[card] -= AS_CHUNKSIZE;
	
}

static inline void __wcfxs_proslic_check_hook(struct wcfxs *wc, int card)
{
	char res;
	int hook;	/* 0 : ON_HOOK, 1: OFF-HOOK */

	/* For some reason we have to debounce the hook detector.  */
//printk("ProSlic check hook\r\n");
	res = as_fxs_getreg(wc, card, 68);

	hook = (res&0x02);
	if (  hook  )//&& !wc->mod.fxs.oldrxhook[card] ) 
	{/* debounce result : Off hook status in driver */
#if AS_SLIC_DEBUG
		if (debug)
			printk("wcfxs: Card %d Ring Trip Detect\n", card);
#endif				
		as_io_channel_hooksig( wc->chans[card], AS_RXSIG_OFFHOOK);
	}
	
	hook = (res & 1); /* Loop Closure Detect has occurred, eg has been off-hook */

	/* In IXP4xx, Interrupt is happened in 10 ms duration, so no debounce is needed now, lizhijie,2006.03.10 */
#if 1


	if ( hook&& !wc->mod.fxs.oldrxhook[card] ) 
	{/* debounce result : Off hook status in driver */
#if AS_SLIC_DEBUG
		//if (debug)
			//printk("wcfxs: Card %d Going off hook\n", card);
#endif				
		as_io_channel_hooksig( wc->chans[card], AS_RXSIG_OFFHOOK);

/*add for DP, wangwei ,2006.04.21*/
		if (wc->chans[card]->pulseflag == AS_NO_PPS && robust)
			as_fxs_init_proslic(wc, card, 1, 0, 1);
		
		wc->mod.fxs.oldrxhook[card] = 1;

	} 
	else  if(!hook && wc->mod.fxs.oldrxhook[card])
	{/* debounce result, now On hook status */
#if AS_SLIC_DEBUG
		//if (debug)
			//printk("wcfxs: Card %d Going on hook\n", card);
#endif				
		as_io_channel_hooksig( wc->chans[card],  AS_RXSIG_ONHOOK);
		wc->mod.fxs.oldrxhook[card] = 0;

	}

#else
	if (hook != wc->mod.fxs.lastrxhook[card]) 
	{/* status is not off-hook, so reset the debounce */
	/* Reset the debounce (must be multiple of 4ms : 4 cards in device ,lzj ) */
	/* change as following,2006.01.13 
		wc->mod.fxs.debounce[card] = 8 * (4 * 8);
	*/		
		/* ISR is in 10 ms duration, DP pulse make duration is 15ms, so debounce time must be 1 interrupt duration: 10ms 
		Li zhijie, 2006.03.10 */
		wc->mod.fxs.debounce[card] =  AS_CHUNKSIZE;
#if AS_SLIC_DEBUG
		printk("Resetting debounce card %d hook status :%s,(old %s)\n", card, hook?"OFF-HOOK":"ON-HOOK", wc->mod.fxs.debounce[card]?"OFF-HOOK":"ON-HOOK");
#endif
	} 
	else 
	{
		if (wc->mod.fxs.debounce[card] > 0) 
		{
/*	debounce for 10ms ,lizhijie,2006.03.10 
			wc->mod.fxs.debounce[card]-= 4 * AS_CHUNKSIZE;
*/			
			wc->mod.fxs.debounce[card] -=  AS_CHUNKSIZE;
#if AS_SLIC_DEBUG
			printk("Sustaining hook %s (old %s)\n", hook?"OFF-HOOK":"ON-HOOK", wc->mod.fxs.debounce[card]?"OFF-HOOK":"ON-HOOK");
#endif
			if (!wc->mod.fxs.debounce[card]) 
			{
#if AS_SLIC_DEBUG
				printk("Counted down debounce, newhook: %s...\n", hook?"OFF-HOOK":"ON-HOOK");
#endif
				/* debouncehook is 1, then off-hook*/
				wc->mod.fxs.debouncehook[card] = hook;	/* can be offhook or on-hook */
			}
			
			if (!wc->mod.fxs.oldrxhook[card] && wc->mod.fxs.debouncehook[card]) 
			{/* debounce result : Off hook status in driver */
#if AS_SLIC_DEBUG
				if (debug)
					printk("wcfxs: Card %d Going off hook\n", card);
#endif				
				as_io_channel_hooksig( wc->chans[card], AS_RXSIG_OFFHOOK);
				if (robust)
					as_fxs_init_proslic(wc, card, 1, 0, 1);
				wc->mod.fxs.oldrxhook[card] = 1;
			} 
			else if (wc->mod.fxs.oldrxhook[card] && !wc->mod.fxs.debouncehook[card]) 
			{/* debounce result, now On hook status */
#if AS_SLIC_DEBUG
				if (debug)
					printk("wcfxs: Card %d Going on hook\n", card);
#endif				
				as_io_channel_hooksig( wc->chans[card],  AS_RXSIG_ONHOOK);
				wc->mod.fxs.oldrxhook[card] = 0;
			}
		}
	}
	wc->mod.fxs.lastrxhook[card] = hook;
#endif

}


static inline void __wcfxs_proslic_recheck_sanity(struct wcfxs *wc, int card)
{
	int res;
	/* Check loopback */
	res = as_fxs_getreg(wc, card, 8);
	if (res) 
	{
		printk("Ouch, part reset, quickly restoring reality (card %d, reg8=%x)\n", card, res);
		as_fxs_init_proslic(wc, card, 1, 0, 1);
		
		as_fxs_setreg_nolock(wc, card, 64, AS_LINEFEED_FORWARD_ONHOOK);
	} 
	else 
	{
		res = as_fxs_getreg(wc, card, 64);
		if (!res && (res != wc->mod.fxs.lasttxhook[card])) 
		{
			if (wc->mod.fxs.palarms[card]++ < MAX_ALARMS) 
			{
				printk("Power alarm on module %d, resetting!\n", card + 1);
				if (wc->mod.fxs.lasttxhook[card] == 4)
					wc->mod.fxs.lasttxhook[card] = 1;
				as_fxs_setreg(wc, card, 64, wc->mod.fxs.lasttxhook[card]);
			} 
			else 
			{
				if (wc->mod.fxs.palarms[card] == MAX_ALARMS)
					printk("Too many power alarms on card %d, NOT resetting!\n", card + 1);
			}
		}
	}
}

static inline void __wcfxs_transmitprep(struct wcfxs *wc, unsigned char ints)
{
	volatile unsigned int *writechunk;
	int x;
	
	if (ints & 0x01) /* Write is at interrupt address.  Start writing from normal offset */
	{
		writechunk = wc->writechunk;
#if AS_DEBUG_TIGER
		wc->write_interrupts ++;
#endif
	}
	else if(ints & 0x02) /* Write reach at the end of buffer */
	{
		writechunk = wc->writechunk + AS_CHUNKSIZE;
#if AS_DEBUG_TIGER
		wc->write_ends++;
#endif
	}
	else
		return;

	/* Calculate Transmission */
	as_io_span_out_transmit( wc );

	for (x=0;x<AS_CHUNKSIZE;x++) 
	{
		/* Send a sample, as a 32-bit word */
		writechunk[x] = 0;
#ifdef  __ARM__
		if (wc->cardflag & (1 << 3))
			writechunk[x] |= (wc->chans[3]->writechunk[x] );
		if (wc->cardflag & (1 << 2))
			writechunk[x] |= (wc->chans[2]->writechunk[x] << 8);
		if (wc->cardflag & (1 << 1))
			writechunk[x] |= (wc->chans[1]->writechunk[x] << 16);
		if (wc->cardflag & (1 << 0))
			writechunk[x] |= (wc->chans[0]->writechunk[x]<< 24);
#else
		if (wc->cardflag & (1 << 3))
			writechunk[x] |= (wc->chans[3]->writechunk[x] << 24);
		if (wc->cardflag & (1 << 2))
			writechunk[x] |= (wc->chans[2]->writechunk[x] << 16);
		if (wc->cardflag & (1 << 1))
			writechunk[x] |= (wc->chans[1]->writechunk[x] << 8);
		if (wc->cardflag & (1 << 0))
			writechunk[x] |= (wc->chans[0]->writechunk[x]);
#endif		
	}

}

static inline void __wcfxs_receiveprep(struct wcfxs *wc, unsigned char ints)
{
	volatile unsigned int *readchunk;
	int x;

	if (ints & 0x08) /* Interrupt DMA read reach to the end of buffer */
	{
		readchunk = wc->readchunk + AS_CHUNKSIZE;
#if AS_DEBUG_TIGER
		wc->read_ends ++;
#endif
	}
	else if(ints & 0x04) 	/* Read is at interrupt address.  Valid data is available at normal offset */
	{
		readchunk = wc->readchunk;
#if AS_DEBUG_TIGER
		wc->read_interrupts ++;
#endif
	}
	else
		return;

	for (x=0;x<AS_CHUNKSIZE;x++) 
	{
#ifdef  __ARM__
		if (wc->cardflag & (1 << 3))
			wc->chans[3]->readchunk[x] = (readchunk[x] ) & 0xff;
		if (wc->cardflag & (1 << 2))
			wc->chans[2]->readchunk[x] = (readchunk[x] >> 8) & 0xff;
		if (wc->cardflag & (1 << 1))
			wc->chans[1]->readchunk[x] = (readchunk[x] >> 16) & 0xff;
		if (wc->cardflag & (1 << 0))
			wc->chans[0]->readchunk[x] = (readchunk[x]>> 24) & 0xff;
#else	
		if (wc->cardflag & (1 << 3))
			wc->chans[3]->readchunk[x] = (readchunk[x] >> 24) & 0xff;
		if (wc->cardflag & (1 << 2))
			wc->chans[2]->readchunk[x] = (readchunk[x] >> 16) & 0xff;
		if (wc->cardflag & (1 << 1))
			wc->chans[1]->readchunk[x] = (readchunk[x] >> 8) & 0xff;
		if (wc->cardflag & (1 << 0))
			wc->chans[0]->readchunk[x] = (readchunk[x]) & 0xff;
#endif
	}


#if AS_SOFT_ECHO
	/* XXX We're wasting 8 taps.  We should get closer :( */
	for (x=0;x<wc->cards;x++) 
	{
		if (wc->cardflag & (1 << x))
			as_ec_chunk( wc->chans[x], wc->chans[x]->readchunk, wc->chans[x]->writechunk);
	}
#endif

	as_io_span_in_receive( wc);
}

void as_fxs_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct wcfxs *wc = dev_id;
	unsigned char ints;
	int x;
	int i;
/* refer tiger page 23 */
	ints = inb(wc->ioaddr + WC_INTSTAT);
	outb(ints, wc->ioaddr + WC_INTSTAT);

	if (!ints)
		return;

/* PCI master abort */
	if (ints & 0x10) 
	{
		/* Stop DMA, wait for watchdog */
		printk("TDM PCI Master abort\n");
		as_fxs_stop_dma(wc);
		return;
	}
	if (ints & 0x20)
	{
		printk("TDM PCI Target abort\n");
		return;
	}

	wc->intcount++;
	for (i=0; i<wc->cards; i++) 
	{
		x = wc->cardslot[i];

		if ( (wc->cardflag & (1 << x)) )
		{
			if( (wc->modtype[x] == MOD_TYPE_FXS) ) 
			{
				as_fxs_get_dtmf(wc, x );
				if (wc->mod.fxs.lasttxhook[x] == 0x4)  /*register 64 is 0x04: ringing */
				{
					/* RINGing, prepare for OHT */
					wc->mod.fxs.ohttimer[x] = OHT_TIMER << 3;
					wc->mod.fxs.idletxhookstate[x] = 0x2;	/* OHT mode when idle */
				} 
				else 
				{
					if (wc->mod.fxs.ohttimer[x]) 
					{
						wc->mod.fxs.ohttimer[x]-= AS_CHUNKSIZE;
						if (!wc->mod.fxs.ohttimer[x]) 
						{
							wc->mod.fxs.idletxhookstate[x] = 0x1;	/* Switch to active */
							if (wc->mod.fxs.lasttxhook[x] == 0x2) 
							{
								/* Apply the change if appropriate */
								wc->mod.fxs.lasttxhook[x] = 0x1;
								as_fxs_setreg(wc, x, 64, wc->mod.fxs.lasttxhook[x]);
							}
						}
					}
				}
				
				__wcfxs_proslic_check_hook(wc, x);
				if (!(wc->intcount & 0xfc))
					__wcfxs_proslic_recheck_sanity(wc, x);
			}
			else if (wc->modtype[x] == MOD_TYPE_FXO)
			{
				__wcfxs_voicedaa_check_hook(wc, x);
			}
		}
	}

	if (ints & 0x0f) 
	{
#if 0		
		if (!(wc->intcount % 10000)) 
		{/* following code should be checked , lizhijie 2004.11.13 */
			/* Accept an alarm once per 10 seconds */
			for (i=0;i<wc->cards;i++) 
			{
				x = wc->cardslot[i];
				if (wc->modtype[x] == MOD_TYPE_FXS) 
				{
					if (wc->mod.fxs.palarms[x])
						wc->mod.fxs.palarms[x]--;
				}
			}	
		}
#endif		
		__wcfxs_receiveprep(wc, ints);
		__wcfxs_transmitprep(wc, ints);
	}

}

