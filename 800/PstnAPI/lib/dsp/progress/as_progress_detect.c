/*
 * $Author: lizhijie $
 * $Id: as_progress_detect.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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

#include "as_progress_detect.h"

#include "as_dsp.h"

#define	WITH_TALKING_CHECK		0

#define	PROGRESS_DEBUG			0

#if 0
static struct progalias 
{
	char *name;
	int mode;
} aliases[] = 
{
	{ "jp", PROG_MODE_JAPAN },
	{ "us", PROG_MODE_NA },
};
#endif


/* For JAPAN modes */
#define 	JAPAN_HZ_400				0
#define 	JAPAN_HZ_15				1	/* RING BACK */
#define 	JAPAN_HZ_16				2	/* CALL WAIT */
#define 	JAPAN_HZ_1400				3	/* RECORDER */

#define 	JAPAN_FREQ_NORMAL		400
#define	JAPAN_FREQ_RBT			15		/* another signal modulated to carry 400HZ */
#define	JAPAN_FREQ_IIT				16
#define	JAPAN_FREQ_RECORD		1400

static struct progress 
{
	int 		size;
	int 		freqs[FREQ_SIZE_JAPAN];
} modes[] = 
{
/*
	{ GSAMP_SIZE_JAPAN, { JAPAN_FREQ_NORMAL, JAPAN_FREQ_RBT, JAPAN_FREQ_IIT, JAPAN_FREQ_RECORD  } },
*/
	{ GSAMP_SIZE_JAPAN, { JAPAN_FREQ_NORMAL, 0, 0, 0  } },

};


#define TONE_THRESH 			10.0		/* How much louder the tone should be than channel energy */
#define TONE_MIN_THRESH 		1e8		/* How much tone there should be at least to attempt */
#define COUNT_THRESH  			3		/* Need at least 50ms of stuff to count it */

#define TONE_STATE_SILENCE  	0
#define TONE_STATE_DIALTONE 	1
#define TONE_STATE_RINGING  	2 
#define TONE_STATE_SPECIAL1	3
#define TONE_STATE_BUSY     		4
#define TONE_STATE_TALKING  	5
#define TONE_STATE_SPECIAL2 	6
#define TONE_STATE_SPECIAL3 	7


static inline void __pr_goertzel_sample(goertzel_state_t *s, short sample)
{
	float v1;
	float fsamp  = sample;
	v1 = s->v2;
	s->v2 = s->v3;
	s->v3 = s->fac * s->v2 - v1 + fsamp;
}

static inline void __pr_goertzel_update(goertzel_state_t *s, short *samps, int count)
{
	int i;
	for (i=0;i<count;i++) 
		__pr_goertzel_sample(s, samps[i]);
}


static inline float __pr_goertzel_result(goertzel_state_t *s)
{
	return s->v3 * s->v3 + s->v2 * s->v2 - s->v2 * s->v3 * s->fac;
}

static inline void __pr_goertzel_init(goertzel_state_t *s, float freq, int samples)
{
	s->v2 = s->v3 = 0.0;
	s->fac = 2.0 * cos(2.0 * M_PI * (freq / 8000.0));
}

static inline void __pr_goertzel_reset(goertzel_state_t *s)
{
	s->v2 = s->v3 = 0.0;
}

static void __as_progress_reset(progress_t *dsp)
{
	int x;

	for (x=0; x<dsp->freqcount; x++)
		dsp->freqs[x].v2 = dsp->freqs[x].v3 = 0.0;
	
	dsp->loopCount = 0;
	dsp->toneCount = 0;
	dsp->silenceCount = 0;
	dsp->talkingCount = 0;

	dsp->tstate = TONE_STATE_SILENCE;
	
	dsp->gsamps = 0;
	dsp->genergy = 0.0;
}

static void __as_progress_init(progress_t *dsp)
{
	int max = 0;
	int x;
	dsp->gsamp_size = modes[dsp->progmode].size;
	dsp->gsamps = 0;

	for (x=0;x<sizeof(modes[dsp->progmode].freqs) / sizeof(modes[dsp->progmode].freqs[0]);x++) 
	{
		if (modes[dsp->progmode].freqs[x]) 
		{
			__pr_goertzel_init(&dsp->freqs[x], (float)modes[dsp->progmode].freqs[x], dsp->gsamp_size);
			max = x;
		}
	}
	dsp->freqcount = 1; //max;

#if PROGRESS_DEBUG
	printf("frequency count =%d\n", dsp->freqcount );
#endif

	__as_progress_reset(dsp);
}

#if 0
static inline int __pair_there(float p1,  float e)
{
	/* Make sure absolute levels are high enough */
	if (p1 < TONE_MIN_THRESH) 
		return 0;
	/* Amplify ignored stuff */
	e *= TONE_THRESH;

	if (p1 < e )
		return 0;

	return 1;
}
#endif


static int __as_progress_detect_without_freq_check(progress_t *dsp )
{
	int x;
	int pass;
	int newstate = TONE_STATE_SILENCE;
	int res = 0;
	int len = dsp->length;
	short *s = dsp->buf;
	
	while(len) 
	{
		pass = len;
		if (pass > dsp->gsamp_size - dsp->gsamps) 
			pass = dsp->gsamp_size - dsp->gsamps;

		for (x=0;x<pass;x++) 
		{
			dsp->genergy += s[x] * s[x];
		}
		
		s += pass;
		dsp->gsamps += pass;
		len -= pass;
		
#if PROGRESS_DEBUG
		printf("energy : %d  threshold : %d\n",(int)dsp->genergy, (int)(TONE_MIN_THRESH * TONE_THRESH));
#endif

		if (dsp->gsamps == dsp->gsamp_size) 
		{

//			if (dsp->genergy > TONE_MIN_THRESH * TONE_THRESH) 
			if (dsp->genergy > TONE_MIN_THRESH/TONE_THRESH) 
			{
				newstate = TONE_STATE_DIALTONE;
				dsp->tstate = TONE_STATE_DIALTONE;
			} 
			else
			{
				newstate = TONE_STATE_SILENCE;
			}	

			if ( dsp->tstate == TONE_STATE_DIALTONE ) 
			{
				dsp->loopCount++;
				if(newstate == TONE_STATE_DIALTONE )
					dsp->toneCount++;
				else if(newstate == TONE_STATE_SILENCE )
					dsp->silenceCount++;
				else if(newstate == TONE_STATE_TALKING)
					dsp->talkingCount++;
				
				dsp->tcount++;
			} 
			
			dsp->gsamps = 0;
			dsp->genergy = 0.0;
		}
	}

	return res;
}

#if 1
static int __as_progress_detect(progress_t *dsp )
{
	int x;
	int y;
	int pass;
	int newstate = TONE_STATE_SILENCE;
	int res = 0;
	int len = dsp->length;
	short *s = dsp->buf;
	
	while(len) 
	{
		pass = len;
		if (pass > dsp->gsamp_size - dsp->gsamps) 
			pass = dsp->gsamp_size - dsp->gsamps;

		for (x=0;x<pass;x++) 
		{
			for (y=0; y<dsp->freqcount; y++) 
				__pr_goertzel_sample(&dsp->freqs[y], s[x]);
			dsp->genergy += s[x] * s[x];
		}
		
		s += pass;
		dsp->gsamps += pass;
		len -= pass;
		
		if (dsp->gsamps == dsp->gsamp_size) 
		{
			float hz[FREQ_SIZE_JAPAN];
			for (y=0; y<dsp->freqcount; y++)
				hz[y] = __pr_goertzel_result(&dsp->freqs[y]);
#if 0
			if (__pair_there(hz[JAPAN_HZ_400], dsp->genergy) ) 
			{
				newstate = TONE_STATE_BUSY;
				newstate = TONE_STATE_RINGING;
			} 
#endif			

#if PROGRESS_DEBUG
			printf("energy : %d  400Hz tone :%d  threshold : %d\n",(int)dsp->genergy, (int)hz[JAPAN_HZ_400],(int)(TONE_MIN_THRESH * TONE_THRESH));
#endif
			if (hz[JAPAN_HZ_400] > TONE_MIN_THRESH * TONE_THRESH) 
			{
				newstate = TONE_STATE_DIALTONE;
				dsp->tstate = TONE_STATE_DIALTONE;
			} 
#if WITH_TALKING_CHECK
			else if (dsp->genergy > TONE_MIN_THRESH * TONE_THRESH) 
			{
				newstate = TONE_STATE_TALKING;
				trace;
				return TONE_STATE_TALKING;
			} 
#endif			
			else
			{
				newstate = TONE_STATE_SILENCE;
			}	

			if ( dsp->tstate == TONE_STATE_DIALTONE ) 
			{
				dsp->loopCount++;
				if(newstate == TONE_STATE_DIALTONE )
					dsp->toneCount++;
				else if(newstate == TONE_STATE_SILENCE )
					dsp->silenceCount++;
				else if(newstate == TONE_STATE_TALKING)
					dsp->talkingCount++;
				
				dsp->tcount++;
			} 

#if 0			
			if (newstate == dsp->tstate) 
			{
				dsp->tcount++;
				if (dsp->tcount == COUNT_THRESH) 
				{
					if (dsp->tstate == TONE_STATE_BUSY) 
					{
						res = AST_CONTROL_BUSY;
						dsp->features &= ~DSP_FEATURE_CALL_PROGRESS;
					} 
					else if (dsp->tstate == TONE_STATE_TALKING) 
					{
						res = AST_CONTROL_ANSWER;
						dsp->features &= ~DSP_FEATURE_CALL_PROGRESS;
					} 
					else if (dsp->tstate == TONE_STATE_RINGING)
						res = AST_CONTROL_RINGING;
					else if (dsp->tstate == TONE_STATE_SPECIAL3) 
					{
						res = AST_CONTROL_CONGESTION;
						dsp->features &= ~DSP_FEATURE_CALL_PROGRESS;
					}
					
				}
			} 
			else 
			{
#if 0
				printf("Newstate: %d\n", newstate);
#endif
				dsp->tstate = newstate;
				dsp->tcount = 1;
			}
#endif
			
			/* Reset goertzel */						
			for (x=0; x<dsp->freqcount; x++)
				dsp->freqs[x].v2 = dsp->freqs[x].v3 = 0.0;
			dsp->gsamps = 0;
			dsp->genergy = 0.0;
		}
	}

	return res;
}
#endif

int as_progressdetect( progress_t *dsp, unsigned char *raw, int length,pstn_standard_t standard)
{
	int res = 0;
	int x;
	int duration;
	int silenceLength;
	int toneLength;
	int result = AS_PROGRESS_UNKNOWN;

	for (x=0; x<length; x++) 
	{
		dsp->buf[dsp->bufp] = XLAW(raw[x], dsp->law);
		dsp->bufp++;
		if(dsp->bufp >= dsp->length )
		{/* ignore other data when length is not equal dsp->length */
			break;
		}
	}

	if(dsp->bufp >= dsp->length)
	{
			dsp->bufp = 0;
#if 0
			res = __as_progress_detect(dsp );
#else
			res = __as_progress_detect_without_freq_check(dsp );
#endif

			if(dsp->tstate != TONE_STATE_DIALTONE)
				return AS_PROGRESS_UNKNOWN;
			
			duration = dsp->loopCount*GSAMP_SIZE_JAPAN/8;		/* in the unit of ms */
			silenceLength = dsp->silenceCount*GSAMP_SIZE_JAPAN/8 ;
			toneLength = dsp->toneCount*GSAMP_SIZE_JAPAN/8;
#if PROGRESS_DEBUG
			printf("duration : %d;\ttone : %d;\tsilence : %d\n", duration, toneLength, silenceLength);
#endif
			
#if WITH_TALKING_CHECK
			if (res) 
			{
				result = AS_PROGRESS_TALKING;
			}
			else 
#endif
				if(duration < 400 )	/* minium requirement  for SDT */
			{
				result = AS_PROGRESS_UNKNOWN;
			}
			else if(duration< 500 )
			{/* check SDT */
				if( silenceLength > 200  && toneLength > 250 )
				{/* silence more than 180 ms */
					result = AS_PROGRESS_2DIALTONE;
				}
			}
			else if(duration < 1000)
			{/* check BT */

				if(standard == PSTN_STANDARD_CHINA)
				{
					if( silenceLength > 345 && toneLength > 345)
					{
						result = AS_PROGRESS_BUSYTONE;
					}	
					else
						result = AS_PROGRESS_UNKNOWN;
				}
				else if(standard == PSTN_STANDARD_JAPAN)
				{
					if( silenceLength > 440 && toneLength > 440)
					{
						result = AS_PROGRESS_BUSYTONE;
					}	
					else
						result = AS_PROGRESS_UNKNOWN;
				} 
			}
			else if(duration < 2000 )
			{/* check RBT and DT */
				if( silenceLength < 200 && toneLength> 1800 )
				{
					result = AS_PROGRESS_DIALTONE;
				}
				else if( silenceLength > 750 && toneLength> 750 )
				{
					result = AS_PROGRESS_RINGBACK;
				}
			}
			else if(duration >= 2000 )
			{
				__as_progress_reset( dsp);
			}
	}

	if(result != AS_PROGRESS_UNKNOWN )
	{
#if 1//PROGRESS_DEBUG
		printf("total : %d ms; silence : %d ms; tone : %d ms; talking : %d\n", dsp->loopCount*GSAMP_SIZE_JAPAN/8,
			dsp->silenceCount*GSAMP_SIZE_JAPAN/8,dsp->toneCount*GSAMP_SIZE_JAPAN/8,dsp->talkingCount*GSAMP_SIZE_JAPAN/8);
#endif
		__as_progress_reset( dsp);
	}	

	return result;
}


int as_progressdetect_busy( progress_t *dsp, unsigned char *raw, int length,pstn_standard_t standard)
{
	int res = 0;
	int x;
	int duration;
	int silenceLength;
	int toneLength;
	int result = AS_PROGRESS_UNKNOWN;

	for (x=0; x<length; x++) 
	{
		dsp->buf[dsp->bufp] = XLAW(raw[x], dsp->law);
		dsp->bufp++;
		if(dsp->bufp >= dsp->length )
		{/* ignore other data when length is not equal dsp->length */
			break;
		}
	}

	if(dsp->bufp >= dsp->length)
	{
			dsp->bufp = 0;
#if 1
			res = __as_progress_detect(dsp );
#else
			res = __as_progress_detect_without_freq_check(dsp );
#endif

			if(dsp->tstate != TONE_STATE_DIALTONE)
				return AS_PROGRESS_UNKNOWN;
			
			duration = dsp->loopCount*GSAMP_SIZE_JAPAN/8;		/* in the unit of ms */
			silenceLength = dsp->silenceCount*GSAMP_SIZE_JAPAN/8 ;
			toneLength = dsp->toneCount*GSAMP_SIZE_JAPAN/8;
#if PROGRESS_DEBUG
			printf("duration : %d;\ttone : %d;\tsilence : %d\n", duration, toneLength, silenceLength);
#endif
			
#if WITH_TALKING_CHECK
			if (res) 
			{
				result = AS_PROGRESS_TALKING;
			}
			else 
#endif
				if(duration < 400 )	/* minium requirement  for SDT */
			{
				result = AS_PROGRESS_UNKNOWN;
			}
			else if(duration< 500 )
			{/* check SDT */
				if( silenceLength > 200  && toneLength > 250 )
				{/* silence more than 180 ms */
					result = AS_PROGRESS_2DIALTONE;
				}
			}
			else if(duration < 1000)
			{/* check BT */

				if(standard == PSTN_STANDARD_CHINA)
				{
					if( silenceLength > 345 && toneLength > 345)
					{
						result = AS_PROGRESS_BUSYTONE;
					}	
				}
				else if(standard == PSTN_STANDARD_JAPAN)
				{
					if( silenceLength > 440 && toneLength > 440)
					{
						result = AS_PROGRESS_BUSYTONE;
					}	
				}
				
			}
			else if(duration < 2000 )
			{/* check RBT and DT */
				if( silenceLength < 200 && toneLength> 1800 )
				{
					result = AS_PROGRESS_DIALTONE;
				}
				else if( silenceLength > 750 && toneLength> 750 )
				{
					result = AS_PROGRESS_RINGBACK;
				}
			}
			else if(duration >= 2000 )
			{
				__as_progress_reset( dsp);
			}
	}

	if(result != AS_PROGRESS_UNKNOWN )
	{
#if 1//PROGRESS_DEBUG
		printf("total : %d ms; silence : %d ms; tone : %d ms; talking : %d\n", dsp->loopCount*GSAMP_SIZE_JAPAN/8,
			dsp->silenceCount*GSAMP_SIZE_JAPAN/8,dsp->toneCount*GSAMP_SIZE_JAPAN/8,dsp->talkingCount*GSAMP_SIZE_JAPAN/8);
#endif
		__as_progress_reset( dsp);
	}	

	return result;
}


progress_t *as_dsp_new_progress(as_dsp_t *root)
{
	progress_t *dsp;
	dsp = (progress_t *)malloc(sizeof(progress_t));
	if (!dsp) 
	{
		printf("No memory available\n");
		exit(1);
	}
	memset(dsp, 0, sizeof(progress_t));
	
//	dsp->features = DSP_FEATURE_SILENCE_SUPPRESS;
	dsp->progmode = 0;
	dsp->root = root;
	dsp->law = root->config.law;
	/* Initialize initial DSP progress detect parameters */
	__as_progress_init(dsp);

	dsp->length = AS_DRIVER_BUFFER_LENGTH; //ms*8;
	dsp->buf = (short *)malloc(sizeof( short)* dsp->length );
	if(!dsp->buf )
	{
		printf("No memory available for CALL PROGRESS DETECT\n");
		exit(1);
	}

	return dsp;
}

void as_dsp_destroy_progress(progress_t *dsp)
{
	free(dsp->buf);
	dsp->buf = NULL;
	free(dsp);
	dsp = NULL;
}

