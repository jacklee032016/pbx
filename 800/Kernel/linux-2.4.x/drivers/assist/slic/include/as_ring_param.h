/*
 * $Author: lizhijie $
 * $Log: as_ring_param.h,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
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
#ifndef __AS_SI_RING_PARAM_H__
#define __AS_SI_RING_PARAM_H__

#include "as_fxs.h"


struct as_si_reg_ring_para
{
	int  waveform;
	int  bias_adjust;	
	int  ring_freq;	
	int  ring_amp;
	int  ring_init_phase;
	int  dc_offset;
};


struct as_si_ring_para
{	
	int  channel_no;
	float  ring_vpk;	
	float  dc_offset;	
		
	int	waveform;	
	float  crest_factor;
	
	int  ring_freq;	
	
};

#endif

