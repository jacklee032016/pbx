/*
 * $Log: as_dtmf_detect.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.5  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.4  2006/03/23 05:22:36  wangwei
 * no message
 *
 * Revision 1.3  2006/03/21 09:50:58  lizhijie
 * debug with vpbx program
 *
 * Revision 1.2  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.1  2006/03/20 06:36:11  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/17 07:58:10  lizhijie
 * add busy tone detect and debug problem of DTMF detect
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_dtmf_detect.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/


#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>

#include "assist_lib.h"

#include "as_dsp.h"

#include "as_dtmf_detect.h"


#define	AS_DTMF_PARSE_STEP			102

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
#define FAX_THRESHOLD				8.0e7
#define FAX_2ND_HARMONIC       		2.0     /* 4dB */
#define DTMF_NORMAL_TWIST			6.3     /* 8dB */
#define DTMF_REVERSE_TWIST		((isradio) ? 4.0 : 2.5)     /* 4dB normal */
#define DTMF_RELATIVE_PEAK_ROW	6.3     /* 8dB */
#define DTMF_RELATIVE_PEAK_COL	6.3     /* 8dB */
#define DTMF_2ND_HARMONIC_ROW	((isradio) ? 1.7 : 2.5)     /* 4dB normal */
#define DTMF_2ND_HARMONIC_COL	63.1    /* 18dB */

static as_tone_detection_descriptor_t dtmf_detect_row[4];
static as_tone_detection_descriptor_t dtmf_detect_col[4];

#if WITH_2ND_HARMONIC
static as_tone_detection_descriptor_t dtmf_detect_row_2nd[4];
static as_tone_detection_descriptor_t dtmf_detect_col_2nd[4];
#endif

#if WITH_FAX_DETECT
static as_tone_detection_descriptor_t fax_detect;
static as_tone_detection_descriptor_t fax_detect_2nd;
#endif

float dtmf_row[] =
{
	697.0,  770.0,  852.0,  941.0
};

float dtmf_col[] =
{
	1209.0, 1336.0, 1477.0, 1633.0
};

#if WITH_FAX_DETECT
static float fax_freq = 1100.0;
#endif

static char dtmf_positions[] = "123A" "456B" "789C" "*0#D";

static void __as_goertzel_init(as_goertzel_state_t *s, as_tone_detection_descriptor_t *t)
{
	s->v2 =
	s->v3 = 0.0;
	s->fac = t->fac;
}

void __as_goertzel_update(as_goertzel_state_t *s, short  x[], int samples)
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

static float __as_goertzel_result (as_goertzel_state_t *s)
{
	return s->v3*s->v3 + s->v2*s->v2 - s->v2*s->v3*s->fac;
}

static void __as_dtmf_detect_init (as_dtmf_detect_state_t *s)
{
	int i;
	float theta;

	s->hit1 = 
	s->hit2 = 0;

	for (i = 0;  i < 4;  i++)
	{
		theta = 2.0*M_PI*(dtmf_row[i]/SAMPLE_RATE);
		dtmf_detect_row[i].fac = 2.0*cos(theta);

		theta = 2.0*M_PI*(dtmf_col[i]/SAMPLE_RATE);
		dtmf_detect_col[i].fac = 2.0*cos(theta);

#if WITH_2ND_HARMONIC
		theta = 2.0*M_PI*(dtmf_row[i]*2.0/SAMPLE_RATE);
		dtmf_detect_row_2nd[i].fac = 2.0*cos(theta);

		theta = 2.0*M_PI*(dtmf_col[i]*2.0/SAMPLE_RATE);
		dtmf_detect_col_2nd[i].fac = 2.0*cos(theta);
#endif		
    
		__as_goertzel_init (&s->row_out[i], &dtmf_detect_row[i]);
		__as_goertzel_init (&s->col_out[i], &dtmf_detect_col[i]);

#if WITH_2ND_HARMONIC
		__as_goertzel_init (&s->row_out2nd[i], &dtmf_detect_row_2nd[i]);
		__as_goertzel_init (&s->col_out2nd[i], &dtmf_detect_col_2nd[i]);
#endif

		s->energy = 0.0;
	}

#if WITH_FAX_DETECT
	/* Same for the fax dector */
	theta = 2.0*M_PI*(fax_freq/SAMPLE_RATE);
	fax_detect.fac = 2.0 * cos(theta);
	__as_goertzel_init (&s->fax_tone, &fax_detect);

	/* Same for the fax dector 2nd harmonic */
	theta = 2.0*M_PI*(fax_freq * 2.0/SAMPLE_RATE);
	fax_detect_2nd.fac = 2.0 * cos(theta);
	__as_goertzel_init (&s->fax_tone2nd, &fax_detect_2nd);
#endif

	s->current_sample = 0;
	s->detected_digits = 0;
	s->lost_digits = 0;
	s->digits[0] = '\0';

//	s->mhit = 0;
}


static void __as_dtmf_update(as_dtmf_detect_state_t *state, int isradio)
{
	int i;
	int hit;
	int best_row;
	int best_col;

	float row_energy[4];
	float col_energy[4];
#if WITH_FAX_DETECT
	float fax_energy;
	float fax_energy_2nd;
#endif

#if WITH_FAX_DETECT
	/* Detect the fax energy, too */
	fax_energy = __as_goertzel_result(&state->fax_tone);
#endif

	/* We are at the end of a DTMF detection block */
	/* Find the peak row and the peak column */
	row_energy[0] = __as_goertzel_result (&state->row_out[0]);
	col_energy[0] = __as_goertzel_result (&state->col_out[0]);

	for (best_row = best_col = 0, i = 1;  i < 4;  i++)
	{
		row_energy[i] = __as_goertzel_result (&state->row_out[i]);
		if (row_energy[i] > row_energy[best_row])
			best_row = i;
		col_energy[i] = __as_goertzel_result (&state->col_out[i]);
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
		
        /* ... and second harmonic test */
#if WITH_2ND_HARMONIC
		if (i >= 4   
			&& (row_energy[best_row] + col_energy[best_col]) > 42.0*state->energy
            		&&  __as_goertzel_result (&state->col_out2nd[best_col])*DTMF_2ND_HARMONIC_COL < col_energy[best_col]
			&&  __as_goertzel_result (&state->row_out2nd[best_row])*DTMF_2ND_HARMONIC_ROW < row_energy[best_row])
#else
		if (i >= 4  && (row_energy[best_row] + col_energy[best_col]) > 42.0*state->energy)
#endif
		{
			hit = dtmf_positions[(best_row << 2) + best_col];
            /* Look for two successive similar results */
            /* The logic in the next test is:
               We need two successive identical clean detects, with
	   something different preceeding it. This can work with
	   back to back differing digits. More importantly, it
	   can work with nasty phones that give a very wobbly start
	   to a digit. */
			if (hit == state->hit3  &&  state->hit3 != state->hit2)
			{
//				state->mhit = hit;
				state->digit_hits[(best_row << 2) + best_col]++;
				state->detected_digits++;
				if ( state->current_digits < AS_MAX_DTMF_DIGITS)
				{
					state->digits[state->current_digits++] = hit;
					state->digits[state->current_digits] = '\0';
				}
				else
				{
					state->lost_digits++;
				}
			}
		}
	}
	
#if WITH_FAX_DETECT
	/* in order to optmized performance , following is commented */
	if (!hit && (fax_energy >= FAX_THRESHOLD) && (fax_energy > state->energy * 21.0)) 
	{
		fax_energy_2nd = __as_goertzel_result(&state->fax_tone2nd);
		if (fax_energy_2nd * FAX_2ND_HARMONIC < fax_energy) 
		{
#if 0
			printf("Fax energy/Second Harmonic: %f/%f\n", fax_energy, fax_energy_2nd);
#endif					
				/* XXX Probably need better checking than just this the energy XXX */
			hit = 'f';
			state->fax_hits++;
		} /* Don't reset fax hits counter */
	} 
	else 
	{
		if (state->fax_hits > 5) 
		{
			state->mhit = 'f';
			state->detected_digits++;
			if ( state->current_digits < AS_MAX_DTMF_DIGITS)
			{
				state->digits[ state->current_digits++] = hit;
				state->digits[ state->current_digits] = '\0';
			}
			else
			{
				state->lost_digits++;
			}
		}
		state->fax_hits = 0;
	}
#endif		

	state->hit1 = state->hit2;
	state->hit2 = state->hit3;
	state->hit3 = hit;
	
    /* Reinitialise the detector for the next block */
	for (i = 0;  i < 4;  i++)
	{
		__as_goertzel_init (&state->row_out[i], &dtmf_detect_row[i]);
		__as_goertzel_init (&state->col_out[i], &dtmf_detect_col[i]);
#if WITH_2ND_HARMONIC
		__as_goertzel_init (&state->row_out2nd[i], &dtmf_detect_row_2nd[i]);
		__as_goertzel_init (&state->col_out2nd[i], &dtmf_detect_col_2nd[i]);
#endif
	}

#if WITH_FAX_DETECT
	__as_goertzel_init (&state->fax_tone, &fax_detect);
	__as_goertzel_init (&state->fax_tone2nd, &fax_detect_2nd);
#endif

	state->energy = 0.0;
	state->current_sample = 0;

}

#if 0
static int __as_dtmf_detect (dtmf_t *dsp, int isradio)
{
	float famp;
	float v1;
//	int i;
	int j;
	int sample = 0;
	int limit;
	as_dtmf_detect_state_t *s = &(dsp->state);;

	for (sample = 0;  sample <dsp->length;  sample = limit)
	{
        	/* 102 is optimised to meet the DTMF specs. */
		if (( dsp->length - sample) >= (AS_DTMF_PARSE_STEP- s->current_sample))
			limit = sample + (AS_DTMF_PARSE_STEP - s->current_sample);
		else
			limit = dsp->length;

        /* The following unrolled loop takes only 35% (rough estimate) of the 
           time of a rolled loop on the machine on which it was developed */
		for (j = sample;  j < limit;  j++)
		{
			famp = dsp->buf[j];
	    
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

#if WITH_2ND_HARMONIC
			v1 = s->col_out2nd[0].v2;
			s->col_out2nd[0].v2 = s->col_out2nd[0].v3;
			s->col_out2nd[0].v3 = s->col_out2nd[0].fac*s->col_out2nd[0].v2 - v1 + famp;

			v1 = s->row_out2nd[0].v2;
			s->row_out2nd[0].v2 = s->row_out2nd[0].v3;

			s->row_out2nd[0].v3 = s->row_out2nd[0].fac*s->row_out2nd[0].v2 - v1 + famp;
        
			v1 = s->col_out2nd[1].v2;
			s->col_out2nd[1].v2 = s->col_out2nd[1].v3;
			s->col_out2nd[1].v3 = s->col_out2nd[1].fac*s->col_out2nd[1].v2 - v1 + famp;

			v1 = s->row_out2nd[1].v2;
			s->row_out2nd[1].v2 = s->row_out2nd[1].v3;
			s->row_out2nd[1].v3 = s->row_out2nd[1].fac*s->row_out2nd[1].v2 - v1 + famp;
        
			v1 = s->col_out2nd[2].v2;
			s->col_out2nd[2].v2 = s->col_out2nd[2].v3;
			s->col_out2nd[2].v3 = s->col_out2nd[2].fac*s->col_out2nd[2].v2 - v1 + famp;
        
			v1 = s->row_out2nd[2].v2;
			s->row_out2nd[2].v2 = s->row_out2nd[2].v3;
			s->row_out2nd[2].v3 = s->row_out2nd[2].fac*s->row_out2nd[2].v2 - v1 + famp;
        
			v1 = s->col_out2nd[3].v2;
			s->col_out2nd[3].v2 = s->col_out2nd[3].v3;
			s->col_out2nd[3].v3 = s->col_out2nd[3].fac*s->col_out2nd[3].v2 - v1 + famp;
        
			v1 = s->row_out2nd[3].v2;
			s->row_out2nd[3].v2 = s->row_out2nd[3].v3;
			s->row_out2nd[3].v3 = s->row_out2nd[3].fac*s->row_out2nd[3].v2 - v1 + famp;
#endif

#if WITH_FAX_DETECT
			/* in order to optimized performace, following is commented, lizhijie,2006.01.05 */
			/* Update fax tone */
			v1 = s->fax_tone.v2;
			s->fax_tone.v2 = s->fax_tone.v3;
			s->fax_tone.v3 = s->fax_tone.fac*s->fax_tone.v2 - v1 + famp;

			v1 = s->fax_tone.v2;
			s->fax_tone2nd.v2 = s->fax_tone2nd.v3;
			s->fax_tone2nd.v3 = s->fax_tone2nd.fac*s->fax_tone2nd.v2 - v1 + famp;
#endif

		}
		
		s->current_sample += (limit - sample);
		if (s->current_sample >= AS_DTMF_PARSE_STEP )
		{
			__as_dtmf_update(s, isradio );
//			printf("update State\n");
//			break;
		}	
		
	}

#if 0
	if ((!s->mhit) || (s->mhit != hit))
	{
		s->mhit = 0;
		return(0);
	}
#endif

	return 0;
}
#else
/* return 0 when no DTMF detected; return a int represent the char's ASCII value */
static int __as_dtmf_detect (dtmf_t *dsp, int isradio)
{
	float famp;
	float v1;
//	int i;
	//int j;
	int sample = 0;
	//int limit;
	as_dtmf_detect_state_t *s = &(dsp->state);;

	for (sample = 0;  sample <AS_DTMF_PARSE_STEP;  sample++)//sample = limit)
	{
        	/* 102 is optimised to meet the DTMF specs. */
        /* The following unrolled loop takes only 35% (rough estimate) of the 
           time of a rolled loop on the machine on which it was developed */
			famp = dsp->buf[sample];
	    
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

#if WITH_2ND_HARMONIC
			v1 = s->col_out2nd[0].v2;
			s->col_out2nd[0].v2 = s->col_out2nd[0].v3;
			s->col_out2nd[0].v3 = s->col_out2nd[0].fac*s->col_out2nd[0].v2 - v1 + famp;

			v1 = s->row_out2nd[0].v2;
			s->row_out2nd[0].v2 = s->row_out2nd[0].v3;

			s->row_out2nd[0].v3 = s->row_out2nd[0].fac*s->row_out2nd[0].v2 - v1 + famp;
        
			v1 = s->col_out2nd[1].v2;
			s->col_out2nd[1].v2 = s->col_out2nd[1].v3;
			s->col_out2nd[1].v3 = s->col_out2nd[1].fac*s->col_out2nd[1].v2 - v1 + famp;

			v1 = s->row_out2nd[1].v2;
			s->row_out2nd[1].v2 = s->row_out2nd[1].v3;
			s->row_out2nd[1].v3 = s->row_out2nd[1].fac*s->row_out2nd[1].v2 - v1 + famp;
        
			v1 = s->col_out2nd[2].v2;
			s->col_out2nd[2].v2 = s->col_out2nd[2].v3;
			s->col_out2nd[2].v3 = s->col_out2nd[2].fac*s->col_out2nd[2].v2 - v1 + famp;
        
			v1 = s->row_out2nd[2].v2;
			s->row_out2nd[2].v2 = s->row_out2nd[2].v3;
			s->row_out2nd[2].v3 = s->row_out2nd[2].fac*s->row_out2nd[2].v2 - v1 + famp;
        
			v1 = s->col_out2nd[3].v2;
			s->col_out2nd[3].v2 = s->col_out2nd[3].v3;
			s->col_out2nd[3].v3 = s->col_out2nd[3].fac*s->col_out2nd[3].v2 - v1 + famp;
        
			v1 = s->row_out2nd[3].v2;
			s->row_out2nd[3].v2 = s->row_out2nd[3].v3;
			s->row_out2nd[3].v3 = s->row_out2nd[3].fac*s->row_out2nd[3].v2 - v1 + famp;
#endif

#if WITH_FAX_DETECT
			/* in order to optimized performace, following is commented, lizhijie,2006.01.05 */
			/* Update fax tone */
			v1 = s->fax_tone.v2;
			s->fax_tone.v2 = s->fax_tone.v3;
			s->fax_tone.v3 = s->fax_tone.fac*s->fax_tone.v2 - v1 + famp;

			v1 = s->fax_tone.v2;
			s->fax_tone2nd.v2 = s->fax_tone2nd.v3;
			s->fax_tone2nd.v3 = s->fax_tone2nd.fac*s->fax_tone2nd.v2 - v1 + famp;
#endif
		
	}
	__as_dtmf_update(s, isradio );

	return 0;
}
#endif

/* get the detected DTMF string from detect_state structure into buf, and 
* return the length because the detected length may less than max parameter 
*/
static int __as_dtmf_get (as_dtmf_detect_state_t *s, char *buf, int max)
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



dtmf_t *as_dsp_new_dtmf(as_dsp_t *root)
{
	dtmf_t *dsp ;

	dsp= (dtmf_t *)malloc(sizeof(dtmf_t) );
	assert( dsp);
	memset(dsp, 0, sizeof(dtmf_t ));

	dsp->root = root;
	dsp->law = root->config.law;
	dsp->length = AS_DRIVER_BUFFER_LENGTH;
	dsp->buf = (short *)malloc( sizeof(short) * dsp->length);
	assert(dsp->buf);

	dsp->lengthOfDigit = DTMF_RAWDATA_LENGTH_4_DIGIT;

	__as_dtmf_detect_init( &(dsp->state) );
	
	return dsp;
}

void as_dsp_destroy_dtmf(dtmf_t *dsp)
{
	free(dsp->buf);
	dsp->buf = 0;

	free(dsp);
	dsp = 0;
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
int as_dtmf_detect(dtmf_t *dsp, unsigned char *sample, int length, char *callId, int max)
{
	int  i;
	int  res = 0;

	for(i=0; i< length; i++)
	{
		dsp->buf[dsp->bufp++] = XLAW(sample[i], dsp->law);
		dsp->position++;
		if(dsp->bufp >= dsp->length )
		{
			dsp->bufp = 0;
			__as_dtmf_detect( dsp, 0 );
//			if( dsp->position >= dsp->lengthOfDigit )
			{
				res= __as_dtmf_get(&(dsp->state), callId, max);
				dsp->position = 0;
			}
		}
	}

	return res;
}

