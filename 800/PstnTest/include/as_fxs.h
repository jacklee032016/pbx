/*
 * $Author: lizhijie $
 * $Log: as_fxs.h,v $
 * Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
 * AS800 PstnTest
 *
 * Revision 1.1  2006/03/13 10:17:33  wangwei
 * no message
 *
 * Revision 1.2  2006/03/13 09:17:56  wangwei
 * no message
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
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
 * ÐÞ¸Ä°æ
 *
 * Revision 1.1.1.2  2004/12/31 08:00:09  fengshikui
 * ÐÞ¸Ä°æ
 *
 * Revision 1.1.1.1  2004/11/18 07:00:04  lizhijie
 * driver for assist telephone cards Tiger320-Si3210/3050
 *
 * $Revision: 1.1.1.1 $
*/
#ifndef  __AS_FXS_H__
#define __AS_FXS_H__
/*
 * Wilcard S100P FXS Interface Driver for Zapata Telephony interface
 */

#include <linux/ioctl.h>

/* added 2004.11.17 */
#define RING_WAVEFORM_SINE	1
#define RING_WAVEFORM_TRAP	2


/* switch phone back to FXS port */
#define	AS_SWITCH_2_FXS				0
/* switch phone from FXS port to Line */
#define	AS_SWITCH_2_LINE				1


#define AS_WITH_FXO_MODULE	1

#define NUM_REGS	  109
#define NUM_INDIRECT_REGS 105

struct wcfxs_stats 
{
	int tipvolt;	/* TIP voltage (mV) */
	int ringvolt;	/* RING voltage (mV) */
	int batvolt;	/* VBAT voltage (mV) */
};

struct wcfxs_regs 
{
	unsigned char direct[NUM_REGS];
	unsigned short indirect[NUM_INDIRECT_REGS];
};

struct wcfxs_regop 
{
	int indirect;
	unsigned char reg;
	unsigned short val;
};

#define WCFXS_GET_STATS	_IOR (AS_CTL_CODE, 60, struct wcfxs_stats)
#define WCFXS_GET_REGS		_IOR (AS_CTL_CODE, 61, struct wcfxs_regs)
#define WCFXS_SET_REG		_IOW (AS_CTL_CODE, 62, struct wcfxs_regop)
/* added by assist , 2004.11.17 */
#define WCFXS_SET_RING_PARA _IOW (AS_CTL_CODE, 63, struct as_si_reg_ring_para)

#define AS_SWITCH_PHONE	_IOR (AS_CTL_CODE, 64, int)

#endif

