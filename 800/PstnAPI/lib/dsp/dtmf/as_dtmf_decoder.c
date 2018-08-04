/*
 * $Log: as_dtmf_decoder.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.2  2006/05/30 03:06:46  lizhijie
 * add newly created files for optimized DTMF algorithm
 *
 * Revision 1.1  2006/05/30 03:05:04  lizhijie
 * no message
 *
 * $Id: as_dtmf_decoder.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/


#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>

#include "assist_lib.h"

#include "as_dsp.h"

#include "as_dtmf_decoder.h"

#define		__T1								0	/* ignored length after MARK, in unit of 20ms */
#define		__T2								0	/* ignored length after SPARE, in unit of 20ms */

#define	DTMF_DECODING_DEBUG					1

#define	DTMF_TWIST_ISRADIO					0


/* Basic DTMF specs:
 *
 * Minimum tone on = 40ms
 * Minimum tone off = 50ms
 * Maximum digit rate = 10 per second
 * Normal twist <= 8dB accepted
 * Reverse twist <= 4dB accepted
 * S/N >= 15dB will detect OK
 * Attenuation <= 26dB will detect OK
 * Frequency tolerance +- 1.5% will detect, +-3.5% will reject
 */


#define SAMPLE_RATE					8000.0

#define DTMF_THRESHOLD				8.0e7
#define DTMF_NORMAL_TWIST			6.3     /* 8dB */
#define DTMF_REVERSE_TWIST		((isradio) ? 4.0 : 2.5)     /* 4dB normal */
#define DTMF_RELATIVE_PEAK_ROW	6.3     /* 8dB */
#define DTMF_RELATIVE_PEAK_COL	6.3     /* 8dB */

static as_dtmf_descriptor_t 		_dtmf_row[4];
static as_dtmf_descriptor_t 		_dtmf_col[4];

static float _dtmf_freq_row[] =
{
	697.0,  770.0,  852.0,  941.0
};

static float _dtmf_freq_col[] =
{
	1209.0, 1336.0, 1477.0, 1633.0
};


static char _dtmf_digit_list[] = "123A" "456B" "789C" "*0#D";

static void __as_ginit(as_gstate_t *s, as_dtmf_descriptor_t *t)
{
	s->v2 =
	s->v3 = 0.0;
	s->fac = t->fac;
}

void __as_gupdate(as_gstate_t *s, short  x[], int samples)
{
	int i;
	float v1;
    
	for (i = 0;  i < samples;  i++)
	{
		v1 = s->v2;
		s->v2 = s->v3;
		s->v3 = s->fac*s->v2 - v1 + x[i];
	}
}

static float __as_gresult (as_gstate_t *s)
{
	return s->v3*s->v3 + s->v2*s->v2 - s->v2*s->v3*s->fac;
}

static void __as_dtmf_decoder_init (as_dtmf_state_t *s)
{
	int i;
	float theta;

	s->hit1 = 
	s->hit2 = 0;

	for (i = 0;  i < 4;  i++)
	{
		theta = 2.0*M_PI*(_dtmf_freq_row[i]/SAMPLE_RATE);
		_dtmf_row[i].fac = 2.0*cos(theta);

		theta = 2.0*M_PI*(_dtmf_freq_col[i]/SAMPLE_RATE);
		_dtmf_col[i].fac = 2.0*cos(theta);

		__as_ginit (&s->row_out[i], &_dtmf_row[i]);
		__as_ginit (&s->col_out[i], &_dtmf_col[i]);

		s->energy = 0.0;
	}

	s->detected_digit = 0;
	s->digits[0] = '\0';

}

typedef enum
{
	dtmf_result_continue = 0,
	dtmf_result_one_digit,
	dtmf_result_maybe ,
}dtmf_update_result_t;


static int __as_dtmf_decoder_update(as_dtmf_state_t *state, int isradio)
{
	int i;
	int hit;
	int best_row;
	int best_col;

	float row_energy[4];
	float col_energy[4];
	int res = dtmf_result_continue;

	row_energy[0] = __as_gresult (&state->row_out[0]);
	col_energy[0] = __as_gresult (&state->col_out[0]);

	for (best_row = best_col = 0, i = 1;  i < 4;  i++)
	{
		row_energy[i] = __as_gresult (&state->row_out[i]);
		if (row_energy[i] > row_energy[best_row])
			best_row = i;
		col_energy[i] = __as_gresult (&state->col_out[i]);
		if (col_energy[i] > col_energy[best_col])
			best_col = i;
	}

	hit = 0;
    /* Basic signal level test and the twist test */
	if (row_energy[best_row] >= DTMF_THRESHOLD
		&&  col_energy[best_col] >= DTMF_THRESHOLD
		&&  col_energy[best_col] < row_energy[best_row]*DTMF_REVERSE_TWIST
		&&  col_energy[best_col]*DTMF_NORMAL_TWIST > row_energy[best_row])
	{
        /* Relative peak test */
		for (i = 0;  i < 4;  i++)
		{
			if ((i != best_col  &&  col_energy[i]*DTMF_RELATIVE_PEAK_COL > col_energy[best_col])
				||    (i != best_row  &&  row_energy[i]*DTMF_RELATIVE_PEAK_ROW > row_energy[best_row]))
			{
				break;
			}
		}
		
		if (i >= 4  && (row_energy[best_row] + col_energy[best_col]) > 42.0*state->energy)
		{
			hit = _dtmf_digit_list[(best_row << 2) + best_col];
#if DTMF_DECODING_DEBUG
			printf("Hit '%c' \n", hit);
#endif
		}
	}

	if(hit !=0 && hit ==state->hit3 && state->currentStage== AS_DTMF_STAGE_SPARE)//&& state->hit3==state->hit2 ) 
//	if(hit !=0 && state->currentStage== AS_DTMF_STAGE_SPARE)//&& state->hit3==state->hit2 ) 
	{
		state->detected_digit = hit;
		state->lastStage = state->currentStage;
		
		state->currentStage = AS_DTMF_STAGE_MARK;
		res = dtmf_result_maybe;
	}	
	else if(hit==0 && hit == state->hit3 && state->currentStage == AS_DTMF_STAGE_MARK)//&& state->hit3==state->hit2  )
	{
		if(state->currentStage == AS_DTMF_STAGE_MARK && state->detected_digit != 0)
		{
			state->digits[state->current_digits++] = state->detected_digit;
			state->digits[state->current_digits] = '\0';
			state->detected_digit = 0;

			res = dtmf_result_one_digit;
		}
		state->lastStage = state->currentStage;
		state->currentStage = AS_DTMF_STAGE_SPARE;
	}

//state->hit1 = state->hit2;
	state->hit2 = state->hit3;
	state->hit3 = hit;
	
	for (i = 0;  i < 4;  i++)
	{
		__as_ginit (&state->row_out[i], &_dtmf_row[i]);
		__as_ginit (&state->col_out[i], &_dtmf_col[i]);

	}

	state->energy = 0.0;

	return res;
}

/* return 0 when no DTMF detected; return a int represent the char's ASCII value */
static int __as_dtmf_decoder_detect (as_dtmf_decoder_t *dsp, int isradio)
{
	float famp;
	float v1;
	int i;
	int res;
	int sample = 0;
	as_dtmf_state_t *s = &(dsp->state);

	s->hit1= s->hit2 = s->hit3 = 0;
#if DTMF_DECODING_DEBUG
//	printf("\nDTMF decoding %d loops\n", s->current_time/dsp->sizeofBuffer );
#endif
	for(i=0; i< (dsp->sizeofBuffer/DTMF_DETECT_STEP_SIZE); i++)
	{

#if DTMF_DECODING_DEBUG
//	printf("\t %d sub-loops", i );
#endif
		for (sample = 0;  sample <DTMF_DETECT_STEP_SIZE;  sample++)
		{
	        	/* 102 is optimised to meet the DTMF specs. */
	        /* The following unrolled loop takes only 35% (rough estimate) of the 
	           time of a rolled loop on the machine on which it was developed */
				famp = dsp->buf[sample + i*DTMF_DETECT_STEP_SIZE];
		    
				s->energy += famp*famp;
		    
	            /* With GCC 2.95, the following unrolled code seems to take about 35%
	               (rough estimate) as long as a neat little 0-3 loop */
				v1 = s->row_out[0].v2;
				s->row_out[0].v2 = s->row_out[0].v3;
				s->row_out[0].v3 = s->row_out[0].fac*s->row_out[0].v2 - v1 + famp;
	    
				v1 = s->col_out[0].v2;
				s->col_out[0].v2 = s->col_out[0].v3;
				s->col_out[0].v3 = s->col_out[0].fac*s->col_out[0].v2 - v1 + famp;
	    
				v1 = s->row_out[1].v2;
				s->row_out[1].v2 = s->row_out[1].v3;
				s->row_out[1].v3 = s->row_out[1].fac*s->row_out[1].v2 - v1 + famp;

				v1 = s->col_out[1].v2;
				s->col_out[1].v2 = s->col_out[1].v3;
				s->col_out[1].v3 = s->col_out[1].fac*s->col_out[1].v2 - v1 + famp;

				v1 = s->row_out[2].v2;
				s->row_out[2].v2 = s->row_out[2].v3;
				s->row_out[2].v3 = s->row_out[2].fac*s->row_out[2].v2 - v1 + famp;

				v1 = s->col_out[2].v2;
				s->col_out[2].v2 = s->col_out[2].v3;
				s->col_out[2].v3 = s->col_out[2].fac*s->col_out[2].v2 - v1 + famp;

				v1 = s->row_out[3].v2;
				s->row_out[3].v2 = s->row_out[3].v3;
				s->row_out[3].v3 = s->row_out[3].fac*s->row_out[3].v2 - v1 + famp;

				v1 = s->col_out[3].v2;
				s->col_out[3].v2 = s->col_out[3].v3;
				s->col_out[3].v3 = s->col_out[3].fac*s->col_out[3].v2 - v1 + famp;
			
		}
		
		res = __as_dtmf_decoder_update(s, isradio );
		if( res == dtmf_result_one_digit )
		{/* ignore other data in buffer */
			return 1;
		}
		else if(res == dtmf_result_maybe)
			return 0;

	}
	return 0;
}

/* get the detected DTMF string from detect_state structure into buf, and 
* return the length because the detected length may less than max parameter 
*/
static int __as_dtmf_decoder_get (as_dtmf_state_t *s, char *buf, int max)
{
	if (max > s->current_digits)
		max = s->current_digits;

	if (max > 0)
	{
		memcpy (buf, s->digits, max);
		memmove (s->digits, s->digits + max, s->current_digits - max);
		s->current_digits -= max;
	}
	buf[max] = '\0';

//	printf("max=%d\n", max );
	return  max;
}


/* return :
	0 : no dtmd caller id detect, 
	>0, numbr of detected caller ID,
	<0, error
 Parameters :
 	length : should be the times of DTMF_DETECT_STEP_SIZE
 	callerID : keep the detected caller ID, must allocate in advanced
 	max : max length of caller ID you want get 
*/
int as_dtmf_decoding(as_dtmf_decoder_t *dsp, unsigned char *sample, int length, char *callId, int max)
{
	int  i;
	int  res = 0;
	int	ignoreLength = 0;
	as_dtmf_state_t *state = &(dsp->state);

#if DTMF_DECODING_DEBUG
//	printf("Decoding length is %d\n", length );
#endif

	if(length <= state->ignoreLength )
	{
		state->ignoreLength -= length;
		return 0;
	}
	ignoreLength = state->ignoreLength;
	for(i = ignoreLength; i< length; i++)
	{

		dsp->buf[dsp->bufp++] = XLAW(sample[i], dsp->law);
		if(dsp->bufp >= dsp->sizeofBuffer )
		{
			dsp->bufp = 0;
			if( __as_dtmf_decoder_detect( dsp, DTMF_TWIST_ISRADIO ) )
			{
				res++;
			}

			if(state->currentStage == AS_DTMF_STAGE_MARK)
			{/* 2 same hits in last period */
				state->ignoreLength = __T1*AS_DRIVER_BUFFER_LENGTH;	/*100 ms*/;
			}
			else if(state->currentStage == AS_DTMF_STAGE_SPARE)
			{/* no any hit in last period */
				state->ignoreLength = __T2*AS_DRIVER_BUFFER_LENGTH;	/*60 ms*/
			}

			if((length -i) > state->ignoreLength )
			{
				i += state->ignoreLength;
				state->ignoreLength = 0;
			}
			else
			{
				state->ignoreLength -= (length -i);
				i = length;
			}

		}
	}

	if(res != 0)
		return __as_dtmf_decoder_get(state, callId, max);
	return 0;
}

void as_dtmf_reset_decoder(as_dtmf_decoder_t *dsp)
{
	as_dtmf_state_t *state = &(dsp->state);
	dsp->bufp = 0;

	state->currentStage = AS_DTMF_STAGE_SPARE;
	state->lastStage = AS_DTMF_STAGE_SPARE;
	state->ignoreLength = 0;
	
	state->current_digits = 0;
	state->detected_digit = 0;
	state->hit4 = state->hit3 = state->hit2 = state->hit1 = 0;

	return ;
}

as_dtmf_decoder_t *as_dtmf_new_decoder(as_dsp_t *root)
{
	as_dtmf_decoder_t *dsp ;

	dsp= (as_dtmf_decoder_t *)malloc(sizeof(as_dtmf_decoder_t) );
	assert( dsp);
	memset(dsp, 0, sizeof(as_dtmf_decoder_t));

	dsp->root = root;
	dsp->law = root->config.law;
	dsp->sizeofBuffer = AS_DTMF_DECOCDER_BUFFER_LENGTH;
	dsp->buf = (short *)malloc( sizeof(short) * dsp->sizeofBuffer);
	assert(dsp->buf);

	__as_dtmf_decoder_init( &(dsp->state) );
	
	return dsp;
}

void as_dtmf_destroy_decoder(as_dtmf_decoder_t *dsp)
{
	free(dsp->buf);
	dsp->buf = 0;

	free(dsp);
	dsp = 0;
}

