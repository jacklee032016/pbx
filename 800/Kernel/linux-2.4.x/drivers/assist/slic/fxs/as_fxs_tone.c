/*
 * $Author: lizhijie $
 * $Revision: 1.1.1.1 $
 * $Log: as_fxs_tone.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.4  2006/04/25 05:36:36  wangwei
 * no message
 *
 * Revision 1.3  2006/03/12 11:09:32  lizhijie
 * debug DTMF caller ID and tone play after ISR timer is modified
 *
 * Revision 1.2  2006/01/18 13:01:17  lizhijie
 * dd
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.2  2004/11/25 07:59:24  lizhijie
 * recommit all
 *
*/
#include "asstel.h"
#include "as_fxs_common.h"

int as_fxs_play_tone( struct as_dev_chan *chan)
{ 
	//unsigned char flags;
	if( (chan->sig &__AS_SIG_FXS)==__AS_SIG_FXS )
		return -EINVAL;
	
	struct as_tone *tone = chan->curtone;
	struct wcfxs *wc = chan->private;

	if(!tone)
		return -1;

//	chan->curtone=chan->curtone->next;

/* disable oscillator 1 and 2 control register */
       as_fxs_setreg_nolock(wc,  chan->chanpos-1, 32, 0);
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 33, 0);

      	//as_fxs_setreg_nolock(wc,  chan->chanpos-1 ,31, 0x1);
       //as_fxs_setreg_nolock(wc,  chan->chanpos-1 ,20, 0);

	/* frequncy */
	__wcfxs_proslic_setreg_indirect(wc,  chan->chanpos-1, 13, tone->fac1 );
	__wcfxs_proslic_setreg_indirect(wc,  chan->chanpos-1, 16, tone->fac2 );
	/* amplitude */
	__wcfxs_proslic_setreg_indirect(wc,  chan->chanpos-1, 14, tone->init_v2_1 );       
	__wcfxs_proslic_setreg_indirect(wc,  chan->chanpos-1, 17, tone->init_v2_2 );
	/* init phase */
	__wcfxs_proslic_setreg_indirect(wc,  chan->chanpos-1, 15, tone->init_v3_1 );
	__wcfxs_proslic_setreg_indirect(wc,  chan->chanpos-1, 18, tone->init_v3_2 );

#if 0
       /* Oscilator 1 active timer */
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 36, tone->tonesamples&0xff );	
	as_fxs_setreg_nolock(wc,  chan->chanpos-1 ,37, (tone->tonesamples>>8) &0xff);
	/* Oscilator 2 active timer */
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 40, tone->tonesamples&0xff);
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 41, (tone->tonesamples>>8) &0xff );
#else
	/* now ISR is in 10 ms */
       /* Oscilator 1 active timer */
#if 1
{
	unsigned short  tonesamples;
	
	/* get time unit of millseconds , lizhijie, 2006.03.12 */
	tonesamples = tone->tonesamples;//*AS_TIME_UNIT/AS_CHUNKSIZE *1000/125;

#if 0
	unsigned char hreg;
	unsigned char lreg;
#endif

	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 36, (tonesamples)&0xff );	
	as_fxs_setreg_nolock(wc,  chan->chanpos-1 ,37, (tonesamples>>8) &0xff);
#if 0
	lreg = as_fxs_getreg_nolock( wc, chan->chanpos-1, 36);
	hreg = as_fxs_getreg_nolock( wc, chan->chanpos-1, 37);
	printk("sample =%d  calcaulate =%d OSC timer %d\n",tone->tonesamples, tonesamples, lreg + (hreg<<8) );
#endif
	/* Oscilator 2 active timer */
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 40, tonesamples&0xff);
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 41, (tonesamples>>8) &0xff );
}
#else	
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 37, *(timer+3) );	
	as_fxs_setreg_nolock(wc,  chan->chanpos-1 ,36, *(timer+2));
	/* Oscilator 2 active timer */
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 41, *(timer+3) );
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 40, *(timer+2) );
#endif

#endif

#if 0
	/* Active Timer Interrupt is not required , Li Zhijie, 2006.03.12 */
       /*enable the oscillator 1 active timer interrupt*/   		
	flags=as_fxs_getreg_nolock(wc,  chan->chanpos-1, 21);
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 21, flags|0xc1);
#endif


	/* refer to Si3210 datasheet p.77 ,simplify the operation as following, Li Zhijie, 2006.03.12 */
#define OSC_TONE_RX			0x02
#define OSC_ENABLE			(1<<2)
#define OSC_ENABLE_ACTIVE	(1<<4)
#define OSC_OZ				(1<<5)
#if 0
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 32, 0xb7);
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 33, 0xb7);
#endif

	/*start ringtone*/	
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 32, OSC_TONE_RX|OSC_ENABLE|OSC_ENABLE_ACTIVE);//|OSC_OZ);
	as_fxs_setreg_nolock(wc,  chan->chanpos-1, 33, OSC_TONE_RX|OSC_ENABLE|OSC_ENABLE_ACTIVE);//|OSC_OZ);


#if 0	
	 chan->hardtime=chan->curtone->time;
	 if(chan->dialing==1)
	 {
	 	printk("ffsdfsdf%d",starttone);
	 	if(chan->start==0)
			chan->dialing=0;
	 }
#endif

	return 0;
}
	/*add over by ccx 2004.11.3*/
