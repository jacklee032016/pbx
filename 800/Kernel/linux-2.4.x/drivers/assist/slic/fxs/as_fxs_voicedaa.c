/*
 * $Author: lizhijie $
 * $Revision: 1.1.1.1 $
 * $Log: as_fxs_voicedaa.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.5  2006/04/19 12:33:50  lizhijie
 * add CAR support for both FXS/FXO and debugs
 *
 * Revision 1.4  2006/04/19 12:31:48  lizhijie
 * no message
 *
 * Revision 1.3  2006/01/18 13:01:17  lizhijie
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
 * Revision 1.2  2004/11/25 07:59:24  lizhijie
 * recommit all
 *
*/
#include "as_fxs_common.h"
extern int debug;
extern struct fxo_mode  fxo_modes[];
extern int _opermode ;

#define	AS_FXO_FAST_OFFHOOK		0

static int __wcfxs_voicedaa_insane(struct wcfxs *wc, int card)
{
	int blah;
	blah = as_fxs_getreg(wc, card, 2);
	if (blah != 0x3)
		return -2;
	blah = as_fxs_getreg(wc, card, 11);
#if AS_SLIC_DEBUG
	if (debug)
		printk("VoiceDAA System(3050): %02x\n", blah & 0xf);
#endif	
	return 0;
}

int as_fxo_enable_fast_offhook(struct wcfxs *wc, int card)
{

	as_fxs_setreg(wc, card, 31, 0xE2);		/* off-hook counter timer,disabale voltage below 3 v  */
	as_fxs_setreg(wc, card, 25, 0x60);		/* resister calibration */
	as_fxs_setreg(wc, card, 17, 0x20);		/* ADC Calibration */
	return 0;
}


/* sane is 1 : no check is needed */
int as_fxs_init_voicedaa(struct wcfxs *wc, int card, int fast, int manual, int sane)
{
	long newjiffies;
	unsigned char reg16=0, reg26=0, reg30=0, reg31=0;

	/* added for ring validate ,lzj, 2006.04.15 */
	unsigned char  reg24, reg22, reg23;
	
	wc->modtype[card] = MOD_TYPE_FXO;
	/* Sanity check the ProSLIC */
	as_reset_spi(wc, card);
	if (!sane && __wcfxs_voicedaa_insane(wc, card))
		return -2;

	/* Software reset */
	as_fxs_setreg(wc, card, 1, 0x80);

	/* Wait just a bit */
	newjiffies = jiffies + (HZ/10);
	while(jiffies < newjiffies);

	/* Enable PCM, ulaw */
	as_fxs_setreg(wc, card, 33, 0x28);

	/* Set On-hook speed, Ringer impedence, and ringer threshold */
	reg16 |= (fxo_modes[_opermode].ohs << 6);
	reg16 |= (fxo_modes[_opermode].rz << 1);

#define	AS_FXO_RING_THRESOLD_RT2	0x10
#define	AS_FXO_RING_THRESOLD_RT1	0x01
	/*lzj, 2006.04.15 ,40.5Vrms and 49.5. Vrms*/
	reg16 |= AS_FXO_RING_THRESOLD_RT1;//(fxo_modes[_opermode].rt);
	as_fxs_setreg(wc, card, 16, reg16);

#define	AS_FXO_ADC_CALI_DISABLE			0x20	/* ADC Calibration : reg 17 */
#define	AS_FXO_RESIST_CALI_DISABLE		0x60	/* Resister Calibration : reg 25 */
#define	AS_FXO_OFFHOOK_COUNTER_8MS		0x03	/* 8 ms of off-hook counter, reg 31 */

#if AS_FXO_FAST_OFFHOOK
	as_fxs_setreg(wc, card, 17, AS_FXO_ADC_CALI_DISABLE|AS_FXO_RING_THRESOLD_RT2);	
	as_fxs_setreg(wc, card, 25, 0x60);		/* resister calibration */
#else
	as_fxs_setreg(wc, card, 17, AS_FXO_RING_THRESOLD_RT2);	
#endif
	
	/* Set DC Termination:
	   Tip/Ring voltage adjust, minimum operational current, current limitation */
	reg26 |= (fxo_modes[_opermode].dcv << 6);
	reg26 |= (fxo_modes[_opermode].mini << 4);
	reg26 |= (fxo_modes[_opermode].ilim << 1);

	as_fxs_setreg(wc, card, 26, reg26);

	/* Set AC Impedence */
	reg30 = (fxo_modes[_opermode].acim);

	as_fxs_setreg(wc, card, 30, reg30);

	/* Misc. DAA parameters */
//	reg31 = 0xa3;
	reg31=0xa2;

	reg31 |= (fxo_modes[_opermode].ohs2 << 3);
#if AS_FXO_FAST_OFFHOOK
	reg31 |= (AS_FXO_OFFHOOK_COUNTER_8MS<<5);
#endif
	as_fxs_setreg(wc, card, 31, reg31);

	/* Set Transmit/Receive timeslot */
	as_fxs_setreg(wc, card, 34, (3-card) * 8);
	as_fxs_setreg(wc, card, 35, 0x00);
	as_fxs_setreg(wc, card, 36, (3-card) * 8);
	as_fxs_setreg(wc, card, 37, 0x00);

	/* set Ring Validation and Ring Wave, datasheet p.71 ,lizhijie,2006.03.25 */
	as_fxs_setreg(wc, card, 18, 0x02);	/* Full_Wave*/

#define	FXO_REG24_RNGV	0x80
	/*lzj,2005.04.15 ,p.71 ,enable ringer validation */
	reg24 = as_fxs_getreg( wc, card, 24);
	as_fxs_setreg(wc, card, 24, reg24|(FXO_REG24_RNGV) );
	/* validated Ring Envelope, not ring threshold crossing one-shot */
	as_fxs_setreg(wc, card, 18, 0x00);

	/* RDLY : 0 ; RMX frequency  , 2006.04.15 */
	reg22= (0x00<<6)|0x16;
	as_fxs_setreg( wc, card, 22, reg22);

	/* ring vlidate control ,lzj, 2006.04.15 */
#define 	FXO_RTO_128		0x01

#define	FXO_RCC_100		0x00
#define	FXO_RCC_150		0x01
#define	FXO_RCC_200		0x02
#define	FXO_RCC_256		0x03
#define	FXO_RCC_384		0x04
#define	FXO_RCC_512		0x05
#define	FXO_RCC_640		0x06
#define	FXO_RCC_1024		0x07

	/* RDLY :0 ms; RTO :256 ms ; RCC : 384 ms  */
	reg23 = 0x0 |((FXO_RTO_128*2)<<3) |(FXO_RCC_384 );
	as_fxs_setreg(wc, card, 23, reg23);

	/* enable end interrupt of ring burst */
	as_fxs_setreg(wc, card, 2, (FXO_REG2_RDI_MODE)|0x03);

//	as_fxo_enable_fast_offhook(wc, card);

	/* Enable ISO-Cap */
	as_fxs_setreg(wc, card, 6, 0x00);

if (sane == 0)
{
	/* Wait 1000ms for ISO-cap to come up */
	newjiffies = jiffies;
	newjiffies += 2 * HZ;
	while((jiffies < newjiffies) && !(as_fxs_getreg(wc, card, 11) & 0xf0));

	if (!(as_fxs_getreg(wc, card, 11) & 0xf0)) 
	{
		reg31 = as_fxs_getreg(wc, card, 11);
		printk("VoiceDAA did not bring up ISO link properly!(reg11=0x%x)\n", reg31);
		return -1;
	}
#if AS_SLIC_DEBUG
	if (debug)
		printk("ISO-Cap is now up, Line Side(%s): rev %02x\n", 
		       ((as_fxs_getreg(wc, card, 11) >> 4)==0x01)?"3018":"3019",
		       (as_fxs_getreg(wc, card, 13) >> 2) & 0xf);
#endif
}

	/* Enable on-hook line monitor and default on-hook state */
	as_fxs_setreg(wc, card, 5, 0x08);
	reg31 = as_fxs_getreg( wc, card, 12);
#if AS_SLIC_DEBUG
	printk(KERN_ERR"DAA : reg12= 0x%x\n", reg31);
#endif
	return 0;
		
}

