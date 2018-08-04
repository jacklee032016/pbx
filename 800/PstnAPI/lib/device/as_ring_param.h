/*
 * $Log: as_ring_param.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_ring_param.h,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
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

