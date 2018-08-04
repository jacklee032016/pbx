/*
* $Id: as_progress_detect.h,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
* $Aothor$
*/

#ifndef  __AS_PROGRESS_DETECT_H__
#define __AS_PROGRESS_DETECT_H__
/*
* Common Interface routines for Assist DSP Library
*/

#include "assist_lib.h"

/* Number of goertzels for progress detect */
#define 	GSAMP_SIZE_NA 			183			/* North America - 350, 440, 480, 620, 950, 1400, 1800 Hz */
#define 	GSAMP_SIZE_JAPAN 			160			/* - Only care about 400 Hz */

#define	FREQ_SIZE_JAPAN			4

#define 	PROG_MODE_NA				0
#define 	PROG_MODE_JAPAN			1	


typedef struct 
{
	float 	v2;
	float 	v3;
	float 	fac;
} goertzel_state_t;


typedef struct 
{
	int 					features;
	
	goertzel_state_t 		freqs[FREQ_SIZE_JAPAN];
	int 					freqcount;
	
	int 					gsamps;
	int 					gsamp_size;
	int 					progmode;

	int					loopCount;		/* count of execution of decode algorithm */
	int					toneCount;		/* count of 400 HZ tone */
	int					silenceCount;	/* count of silence */
	int					talkingCount;	/* count of audio talking */

	int					sdtCount;		/* count for SDT */
	int					btCount;			/* count for BT */
	int					rbtCount;		/* count for RBT */
	
	int 					tstate;
	int 					tcount;
	
	float 				genergy;

	short				*buf;//[AS_DRIVER_BUFFER_LENGTH];
	int					length;								/* length of AS_DRIVER_BUFFER_LENGTH */
	int					bufp;

	int					law;									/* U or A law */
	
	as_dsp_t				*root;
}progress_t ;

extern	progress_t 	*as_dsp_new_progress(as_dsp_t *root);
extern	void			as_dsp_destroy_progress(progress_t *dsp);

extern	int 			as_progressdetect( progress_t *dsp, unsigned char *raw, int length,pstn_standard_t standard);
extern	int 			as_progressdetect_busy( progress_t *dsp, unsigned char *raw, int length,pstn_standard_t standard);

#endif

