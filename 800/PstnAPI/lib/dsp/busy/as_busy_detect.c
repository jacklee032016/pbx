/*
 * $Log: as_busy_detect.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.4  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.3  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.2  2006/03/20 06:35:29  lizhijie
 * no message
 *
 * Revision 1.1  2006/03/17 07:56:25  lizhijie
 * no message
 *
 * $Author: lizhijie $
 * $Id: as_busy_detect.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/


#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include "assist_lib.h"
#include "as_busy_detect.h"

#include "as_dsp.h"

/* 
* return 1 : busy tone maybe; 0 : not busy tone definitly
*/
static int __as_dsp_busy_maybe( busy_t *dsp )
{
	int accum = 0;
	int x;
	int res = 0;

	if ( dsp->bufp!= AS_DRIVER_BUFFER_LENGTH)
		return 0;
	
	for (x=0;x<AS_DRIVER_BUFFER_LENGTH; x++) 
		accum += abs(dsp->buf[x]);
	accum /= AS_DRIVER_BUFFER_LENGTH;
	
#if AS_BUSY_DETECT_DEBUG
	printf("accum=%d\n",accum);
#endif

//	if (accum < dsp->energyThreshold ) 
	if (accum > dsp->energyThreshold ) 
	{/* SILENCE : energy level < threshold, it is silence, so silence time add len/8 ms */
		dsp->totalSilence += AS_DRIVER_BUFFER_LENGTH/8;  /* 20 ms */

		if (dsp->totalNoise) 
		{/* from NOISE to SILENCE, so record the NOISE level in last duration */
			memmove(dsp->historicNoise, dsp->historicNoise + 1, (AS_BUSY_HISTORY - 1)*sizeof(dsp->historicNoise[0]));
			dsp->historicNoise[AS_BUSY_HISTORY - 1] = dsp->totalNoise;
			dsp->busyMaybe = 1;
			
			dsp->busyCount ++;
		}
		dsp->totalNoise = 0;
		res = 1;
	} 
	else 
	{/*NOISE :  it is not silence duration, eg. noise */
		dsp->totalNoise += AS_DRIVER_BUFFER_LENGTH/8;

		if (dsp->totalSilence) 
		{/* from SILENCE to NOISE, record silence level in last duration */
			int currentSilence = dsp->historicSilence[AS_BUSY_HISTORY - 1];
			int lastSilence = dsp->historicSilence[AS_BUSY_HISTORY - 2];

			memmove(dsp->historicSilence,	dsp->historicSilence + 1, (AS_BUSY_HISTORY - 1)*sizeof(dsp->historicSilence[0]));
			dsp->historicSilence[AS_BUSY_HISTORY - 1] = dsp->totalSilence;
			
			/* check if the previous sample differs only by AS_BUSY_PERCENT from the one before it */
			if (currentSilence < lastSilence) 
			{
				if (currentSilence + currentSilence/AS_BUSY_PERCENT >= lastSilence)
					dsp->busyMaybe = 1;
				else 
					dsp->busyMaybe = 0;
			} 
			else 
			{
				if (currentSilence - currentSilence/AS_BUSY_PERCENT <= lastSilence)
					dsp->busyMaybe = 1;
				else 
					dsp->busyMaybe = 0;
			}
					
		}
		dsp->totalSilence = 0;
	}
	
	return res;
}

static int __as_dsp_busydetect_decide(busy_t *dsp)
{
	int res = 0, x;
	int avgsilencetime = 0, hitsilence = 0;
	int avgtonetime = 0, hittone = 0;

	if (! __as_dsp_busy_maybe( dsp ) )
		return 0;

	dsp->busyCount = 0;
	for (x=0; x< AS_BUSY_HISTORY; x++) 
	{
		avgsilencetime += dsp->historicSilence[x];
		avgtonetime += dsp->historicNoise[x];
		if(dsp->historicSilence[x] > AS_BUSY_MIN && dsp->historicNoise[x]> AS_BUSY_MIN)
			dsp->busyCount ++;
#if AS_BUSY_DETECT_DEBUG
		printf("SILENCE : %d\t NOISE : %d\n", dsp->historicSilence[x], dsp->historicNoise[x]);
#endif
	}
	
#if AS_BUSY_DETECT_DEBUG
	printf("total SILENCE : %d\t total NOISE : %d \tbusycount :%d\n", avgsilencetime, avgtonetime, dsp->busyCount);
#endif

#if 1
	if(!dsp->busyCount )
		return 0;
	
	avgsilencetime /= dsp->busyCount;
	avgtonetime /= dsp->busyCount;
#else	
	avgsilencetime /= AS_BUSY_CADENCE_THRESHOLD;
	avgtonetime /= AS_BUSY_CADENCE_THRESHOLD;
#endif

	for (x= 0; x< AS_BUSY_HISTORY;x++) 
	{
		if (avgsilencetime > dsp->historicSilence[x]) 
		{
			if (avgsilencetime - (avgsilencetime/AS_BUSY_PERCENT) <= dsp->historicSilence[x])
			{/* history-silence is in the range of avg-10%, then silence-hit++ */
				hitsilence++;
			}	
		} 
		else 
		{
			if (avgsilencetime + (avgsilencetime/AS_BUSY_PERCENT) >= dsp->historicSilence[x])
			{/* history-silence is in the range of avg+10%, then silence-hit++ */
				hitsilence++;
			}
		}

		if (avgtonetime > dsp->historicNoise[x]) 
		{
			if (avgtonetime - (avgtonetime/AS_BUSY_PERCENT) <= dsp->historicSilence[x])
				hittone++;
		}
		else 
		{
			if (avgtonetime + (avgtonetime/AS_BUSY_PERCENT) >= dsp->historicSilence[x])
				hittone++;
		}
	}

#if AS_BUSY_DETECT_DEBUG
	printf("tone hit : %d\t silence hit : %d\n", hittone, hitsilence );
#endif

	if ((hittone >= AS_BUSY_CADENCE_THRESHOLD ) && (hitsilence >= AS_BUSY_CADENCE_THRESHOLD ) 
		&& (avgtonetime >= AS_BUSY_MIN && avgtonetime <= AS_BUSY_MAX) 
		&& (avgsilencetime >= AS_BUSY_MIN && avgsilencetime <= AS_BUSY_MAX) ) 
	{
		if (avgtonetime > avgsilencetime) 
		{
			if (avgtonetime - avgtonetime/(AS_BUSY_PERCENT*2) <= avgsilencetime)
				res = 1;
		} 
		else 
		{
			if (avgtonetime + avgtonetime/(AS_BUSY_PERCENT*2) >= avgsilencetime)
				res = 1;
		}
	}

#if AS_BUSY_DETECT_DEBUG
	if (res)
		printf( "detected busy, avg tone time: %d, avg silence time %d\n", avgtonetime, avgsilencetime);
#endif

	return res;
}

static void __as_dsp_busydetect_reset(busy_t *dsp)
{
	dsp->totalSilence = 0;
	dsp->totalNoise = 0;
	dsp->busyCount = 0;
	dsp->bufp = 0;

	memset(dsp->historicSilence, 0, sizeof(dsp->historicSilence));
	memset(dsp->historicNoise, 0, sizeof(dsp->historicNoise));
}

static int __as_dsp_busydetect_init( busy_t *dsp )
{
	dsp->law = dsp->root->config.law;
	dsp->features = AS_DSP_FEATURE_SILENCE_SUPPRESS;

	dsp->energyThreshold = AS_BUSY_DEFAULT_THRESHOLD;
	dsp->busyCount = AS_BUSY_HISTORY;

	dsp->buf = (unsigned short *)malloc( sizeof(unsigned short) *AS_DRIVER_BUFFER_LENGTH );
	if( !dsp->buf )
	{
		printf("No memory available for busy detect\n");
		exit(1);
	}

	__as_dsp_busydetect_reset(dsp);

	return 0;
}

int as_busydetect(busy_t *dsp, unsigned char *buff,int length)
{
	int x;

	for (x=0;x< length; x++) 
	{
		dsp->buf[dsp->bufp] = XLAW( buff[x], dsp->law );
#if AS_BUSY_DETECT_DEBUG
//		printf("%d ", dsp->buf[dsp->bufp]);
#endif
		dsp->bufp++;
		if(dsp->bufp >= AS_DRIVER_BUFFER_LENGTH )
		{
			if( __as_dsp_busydetect_decide( dsp) )
			{
				__as_dsp_busydetect_reset( dsp);
				return 1;
			}
			dsp->bufp = 0;
		}
	}	
	
#if AS_BUSY_DETECT_DEBUG
//		printf("\n\n\n");
#endif

	return 0;
}

busy_t *as_dsp_new_busydetect(as_dsp_t *root)
{
	busy_t *dsp;
	dsp = malloc(sizeof(busy_t));
	assert(dsp);
	memset(dsp, 0, sizeof(busy_t));
	
	dsp->root = root;
	__as_dsp_busydetect_init(dsp);
	return dsp;
}

void as_dsp_destroy_busydetect(busy_t *dsp)
{
	free(dsp->buf);
	dsp->buf = 0;
	free(dsp);
}


#if 0
/*
*  Following defination is for simple interface of call
*/
static busy_t  busyDecoder;
static int	busyInited = 0;

int as_busy_detect(unsigned char *rawdata, int length)
{
	busy_t  *busy = &busyDecoder;
	if(!busyInited )
	{
		busyInited = 1;
		__as_dsp_busydetect_init(busy, U_LAW_CODE );
	}	
	
	return as_dsp_busydetect(busy, rawdata, length);
}
#endif

