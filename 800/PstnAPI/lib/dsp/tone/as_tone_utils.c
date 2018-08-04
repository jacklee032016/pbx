/*
 * $Log: as_tone_utils.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.6  2006/03/24 11:07:46  lizhijie
 * add a C-language file for DTMF and silence data and compile it into libpstn.so
 *
 * Revision 1.5  2006/03/24 11:06:30  lizhijie
 * no message
 *
 * Revision 1.4  2006/03/23 09:35:32  lizhijie
 * no message
 *
 * Revision 1.3  2006/03/22 05:57:25  lizhijie
 * optimized DSP init routine
 *
 * Revision 1.2  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.1  2006/03/20 06:37:26  lizhijie
 * no message
 *
 * Revision 1.4  2006/03/17 09:26:57  wangwei
 * no message
 *
 * Revision 1.3  2006/03/16 08:41:29  lizhijie
 * add FSK support and FSK test programs in IXP4xx platform
 *
 * Revision 1.2  2006/03/16 08:38:23  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_tone_utils.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

#include <math.h>
#include <sys/types.h>
#include <stdio.h>

#include "assist_lib.h"
#include "as_dsp.h"

typedef struct 	tone_calculator	tone_cal_t;

struct tone_calculator
{
	float						ci;		/* ci for local (non-fsk) tone generation */
	float						cr;		/* cr for local (non-fsk) tone generation */
	float						ci1;		/* ci1 for local (non-fsk) tone generation */
	float						cr1;		/* cr1 for local (non-fsk) tone generation */
};

float as_dsp_get_localtone(tone_cal_t *cal, float ddr, float ddi)
{
	float t;
	
	t =cal->cr*ddr-cal->ci*ddi;
	cal->ci = cal->cr*ddi + cal->ci*ddr;
	cal->cr=t;
	
	t=2.0-(cal->cr*cal->cr+cal->ci*cal->ci);
	cal->cr*=t;
	cal->ci*=t;
	
	return cal->cr;
};

/* get sine sample value with 2 frequencies */
static float as_dsp_get_localtone_df(tone_cal_t *cal, float ddr,float ddi,float ddr1, float ddi1)
{
	float t;
	
	t =cal->cr*ddr-cal->ci*ddi;
	cal->ci=cal->cr*ddi+cal->ci*ddr;
	cal->cr=t;
	
	t=2.0-(cal->cr*cal->cr+cal->ci*cal->ci);
	cal->cr*=t;
	cal->ci*=t;
	
	t =cal->cr1*ddr1-cal->ci1*ddi1;
	cal->ci1=cal->cr1*ddi1+cal->ci1*ddr1;
	cal->cr1=t;
	
	t=2.0-(cal->cr1*cal->cr1+cal->ci1*cal->ci1);
	cal->cr1*=t;
	cal->ci1*=t;
	
	return cal->cr + cal->cr1; 
};

static void as_dsp_tone_reset_coeff(tone_cal_t *cal)
{
	cal->cr = 1.0;	/* initialize cr value for local tone genration */
	cal->ci = 0.0;	/* initialize ci value for local tone generation */
	cal->cr1 = 1.0;	/* initialize cr value for local tone genration */
	cal->ci1 = 0.0;	/* initialize ci value for local tone generation */
}

#if 1
int as_dsp_tone_arbtones(unsigned char *data , float f0 , float f1 , int len , int law )
{
	float	ddr,ddi,ddr1,ddi1;
	int	i ;
	tone_cal_t	cal;
	float			sine;

	ddr = cos(f0*2.0*M_PI/8000.0);
	ddi = sin(f0*2.0*M_PI/8000.0);
	ddr1 = cos(f1*2.0*M_PI/8000.0);
	ddi1 = sin(f1*2.0*M_PI/8000.0);

	as_dsp_tone_reset_coeff( &cal);

	for(i = 0; i < (8 * len); i++) 
	{
		sine = as_dsp_get_localtone_df(&cal, ddr,ddi,ddr1,ddi1);
		//printf("%d ", (short)rint(8192.0 * sine));
		*(data+i)=LINEAR2XLAW((short)rint(8192.0 * sine), law /*U_LAW_CODE*/);
	}	
	//printf("\n" );

	return i;
}

#else
/* accelebrate the rate of g711 data , 'len' must be n*20ms 
* Li Zhijie,2006.03.20 
*/
int as_dsp_tone_arbtones(unsigned char *data , float f0 , float f1 , int len , int law )
{
#define TONE_COPY_TIME			20			/* 20ms */
	float	ddr,ddi,ddr1,ddi1;
	int	i, j;
	tone_cal_t	cal;
	float			sine;
	int count = len/TONE_COPY_TIME;

	ddr = cos(f0*2.0*M_PI/8000.0);
	ddi = sin(f0*2.0*M_PI/8000.0);
	ddr1 = cos(f1*2.0*M_PI/8000.0);
	ddi1 = sin(f1*2.0*M_PI/8000.0);

	as_dsp_tone_reset_coeff( &cal);

	/* calculate the first 20 ms data */
	for(i = 0; i < (8 * TONE_COPY_TIME); i++) 
	{
		sine = as_dsp_get_localtone_df(&cal, ddr,ddi,ddr1,ddi1);
		*(data+i)=LINEAR2XLAW((short)rint(8192.0 * sine), law /*U_LAW_CODE*/);
	}	

	/* copy left data in 20ms step(160bytes) */
	for(j=1; j< count; j++)
	{
		for(i = 0; i < (8 * TONE_COPY_TIME); i++) 
		{
			*(data+i + j*8*TONE_COPY_TIME)= *(data+i);
		}	
	}
	//printf("\n" );

	return i*count;
}

int as_dsp_tone_silence(unsigned char *data , int len , int law )
{
#define TONE_COPY_TIME			20			/* 20ms */
	float	ddr,ddi,ddr1,ddi1;
	int	i, j;
	tone_cal_t	cal;
	float			sine;
	int count = len/TONE_COPY_TIME;

	/* calculate the first 20 ms data */
	for(i = 0; i < (8 * TONE_COPY_TIME); i++) 
	{
		*(data+i)=LINEAR2XLAW((short)rint(8192.0 * 0), law /*U_LAW_CODE*/);
	}	

	/* copy left data in 20ms step(160bytes) */
	for(j=1; j< count; j++)
	{
		for(i = 0; i < (8 * TONE_COPY_TIME); i++) 
		{
			*(data+i + j*8*TONE_COPY_TIME)= *(data+i);
		}	
	}
	//printf("\n" );

	return i*count;
}

#endif

