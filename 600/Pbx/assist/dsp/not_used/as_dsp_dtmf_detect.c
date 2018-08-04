/* 
 * $Id: as_dsp_dtmf_detect.c,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 *   DTMF Receiver module
 *   
 */
#include "assist_globals.h"
#include "assist_dsp.h"
#include "pbx_globals.h"


/* keep the coeffiecent for every frequency element , this is not variable in algorithm */
typedef struct
{
	float fac;
} as_tone_detection_descriptor_t;

static as_tone_detection_descriptor_t dtmf_rows[4];
static as_tone_detection_descriptor_t dtmf_cols[4];
static as_tone_detection_descriptor_t dtmf_rows_2nd[4];
static as_tone_detection_descriptor_t dtmf_cols_2nd[4];
static as_tone_detection_descriptor_t fax_detect;
static as_tone_detection_descriptor_t fax_detect_2nd;


static float dtmf_row[] =
{
	697.0,  770.0,  852.0,  941.0
};

static float dtmf_col[] =
{
	1209.0, 1336.0, 1477.0, 1633.0
};

static float fax_freq = 1100.0;

static char dtmf_positions[] = "123A" "456B" "789C" "*0#D";


static void __as_goertzel_init(as_goertzel_state_t *s, as_tone_detection_descriptor_t *t)
{
	s->v2 =
	s->v3 = 0.0;
	s->fac = t->fac;
}

static float __as_goertzel_result (as_goertzel_state_t *s)
{
	return s->v3*s->v3 + s->v2*s->v2 - s->v2*s->v3*s->fac;
}

static void __as_dsp_dtmf_detect_init (assist_dtmf_detector_t  *dsp)
{
	int i;
	float theta;
	as_dtmf_detect_state_t *s= (&dsp->state);
	
	s->hit1 = 
	s->hit2 = 0;

	for (i = 0;  i < 4;  i++)
	{
		theta = 2.0*M_PI*(dtmf_row[i]/SAMPLE_RATE);
		dtmf_rows[i].fac = 2.0*cos(theta);

		theta = 2.0*M_PI*(dtmf_col[i]/SAMPLE_RATE);
		dtmf_cols[i].fac = 2.0*cos(theta);

		theta = 2.0*M_PI*(dtmf_row[i]*2.0/SAMPLE_RATE);
		dtmf_rows_2nd[i].fac = 2.0*cos(theta);

		theta = 2.0*M_PI*(dtmf_col[i]*2.0/SAMPLE_RATE);
		dtmf_cols_2nd[i].fac = 2.0*cos(theta);
    
		__as_goertzel_init (&(s->row_out[i]),  &(dtmf_rows[i]) );
		__as_goertzel_init (&(s->col_out[i]), &(dtmf_cols[i]) );
		__as_goertzel_init (&(s->row_out2nd[i]), &(dtmf_rows_2nd[i]) );
		__as_goertzel_init (&(s->col_out2nd[i]), &(dtmf_cols_2nd[i]) );

		s->energy = 0.0;
	}

	/* Same for the fax dector */
	theta = 2.0*M_PI*(fax_freq/SAMPLE_RATE);
	fax_detect.fac = 2.0 * cos(theta);
	__as_goertzel_init (&(s->fax_tone), &(fax_detect) );

	/* Same for the fax dector 2nd harmonic */
	theta = 2.0*M_PI*(fax_freq * 2.0/SAMPLE_RATE);
	fax_detect_2nd.fac = 2.0 * cos(theta);
	__as_goertzel_init (&(s->fax_tone2nd) , &(fax_detect_2nd) );

	s->current_sample = 0;
	s->detected_digits = 0;
	s->lost_digits = 0;
	s->digits[0] = '\0';
	s->mhit = 0;

}

/* return 0 when no DTMF detected; return a int represent the char's ASCII value */
static int __as_dsp_dtmf_detect (as_dtmf_detect_state_t *s, int16_t amp[],  int samples, int isradio)
{
	float row_energy[4];
	float col_energy[4];
	float fax_energy;
	float fax_energy_2nd;
	float famp;
	float v1;
	int i;
	int j;
	int sample;
	int best_row;
	int best_col;
	int hit;
	int limit;

	hit = 0;
	for (sample = 0;  sample < samples;  sample = limit)
	{
        /* 102 is optimised to meet the DTMF specs. */
		if ((samples - sample) >= (102 - s->current_sample))
			limit = sample + (102 - s->current_sample);
		else
			limit = samples;

        /* The following unrolled loop takes only 35% (rough estimate) of the 
           time of a rolled loop on the machine on which it was developed */
		for (j = sample;  j < limit;  j++)
		{
			famp = amp[j];
	    
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

			/* Update fax tone */
			v1 = s->fax_tone.v2;
			s->fax_tone.v2 = s->fax_tone.v3;
			s->fax_tone.v3 = s->fax_tone.fac*s->fax_tone.v2 - v1 + famp;

			v1 = s->fax_tone.v2;
			s->fax_tone2nd.v2 = s->fax_tone2nd.v3;
			s->fax_tone2nd.v3 = s->fax_tone2nd.fac*s->fax_tone2nd.v2 - v1 + famp;
		}
		
		s->current_sample += (limit - sample);
		if (s->current_sample < 102)
			continue;

		/* Detect the fax energy, too */
		fax_energy = __as_goertzel_result(&s->fax_tone);
		
        /* We are at the end of a DTMF detection block */
        /* Find the peak row and the peak column */
		row_energy[0] = __as_goertzel_result (&s->row_out[0]);
		col_energy[0] = __as_goertzel_result (&s->col_out[0]);

		for (best_row = best_col = 0, i = 1;  i < 4;  i++)
		{
			row_energy[i] = __as_goertzel_result (&s->row_out[i]);
			if (row_energy[i] > row_energy[best_row])
				best_row = i;
			col_energy[i] = __as_goertzel_result (&s->col_out[i]);
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
			if (i >= 4   
				&& (row_energy[best_row] + col_energy[best_col]) > 42.0*s->energy
                		&&  __as_goertzel_result (&s->col_out2nd[best_col])*DTMF_2ND_HARMONIC_COL < col_energy[best_col]
				&&  __as_goertzel_result (&s->row_out2nd[best_row])*DTMF_2ND_HARMONIC_ROW < row_energy[best_row])
			{
				hit = dtmf_positions[(best_row << 2) + best_col];
                /* Look for two successive similar results */
                /* The logic in the next test is:
                   We need two successive identical clean detects, with
		   something different preceeding it. This can work with
		   back to back differing digits. More importantly, it
		   can work with nasty phones that give a very wobbly start
		   to a digit. */
				if (hit == s->hit3  &&  s->hit3 != s->hit2)
				{
					s->mhit = hit;
					s->digit_hits[(best_row << 2) + best_col]++;
					s->detected_digits++;
					if (s->current_digits < AS_MAX_DTMF_DIGITS)
					{
						s->digits[s->current_digits++] = hit;
						s->digits[s->current_digits] = '\0';
					}
					else
					{
						s->lost_digits++;
					}
				}
			}
		}
		
		if (!hit && (fax_energy >= FAX_THRESHOLD) && (fax_energy > s->energy * 21.0)) 
		{
			fax_energy_2nd = __as_goertzel_result(&s->fax_tone2nd);
			if (fax_energy_2nd * FAX_2ND_HARMONIC < fax_energy) 
			{
#if 0
				printf("Fax energy/Second Harmonic: %f/%f\n", fax_energy, fax_energy_2nd);
#endif					
					/* XXX Probably need better checking than just this the energy XXX */
				hit = 'f';
				s->fax_hits++;
			} /* Don't reset fax hits counter */
		} 
		else 
		{
			if (s->fax_hits > 5) 
			{
				s->mhit = 'f';
				s->detected_digits++;
				if (s->current_digits < AS_MAX_DTMF_DIGITS)
				{
					s->digits[s->current_digits++] = hit;
					s->digits[s->current_digits] = '\0';
				}
				else
				{
					s->lost_digits++;
				}
			}
			s->fax_hits = 0;
		}
		s->hit1 = s->hit2;
		s->hit2 = s->hit3;
		s->hit3 = hit;
		
        /* Reinitialise the detector for the next block */
		for (i = 0;  i < 4;  i++)
		{
			__as_goertzel_init (&s->row_out[i], &(dtmf_rows[i]) );
			__as_goertzel_init (&s->col_out[i], &(dtmf_cols[i]) );
			__as_goertzel_init (&s->row_out2nd[i], &(dtmf_rows_2nd[i]) );
			__as_goertzel_init (&s->col_out2nd[i], &(dtmf_cols_2nd[i] ) );
		}

		__as_goertzel_init (&s->fax_tone, &(fax_detect ));
		__as_goertzel_init (&s->fax_tone2nd, &(fax_detect_2nd) );
		s->energy = 0.0;
		s->current_sample = 0;
	}

	if ((!s->mhit) || (s->mhit != hit))
	{
		s->mhit = 0;
		return(0);
	}
	return (hit);
}

/* get the detected DTMF string from detect_state structure into buf, and 
* return the length because the detected length may less than max parameter 
*/
static int __as_dsp_dtmf_get (as_dtmf_detect_state_t *s, char *buf, int max)
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
int _as_dsp_dtmf_detect_with_buffer(as_dtmf_detector *dsp, unsigned char *buf, int length, char *callerId, int max)
{
	int count;
	int i;
	int position;
	int res;

	assert(dsp!=0);
	assert(buf!=0);

	for(i=0; i<dsp->length; i++)
	{
		dsp->line_samples[i] = FULLXLAW(buf[i], dsp->law);
	}	

//	as_error_msg("\r\nlinesample length =%d\r\n", dsp->bufp);
	dsp->bufp = 0;
	while( (dsp->bufp+DTMF_DETECT_STEP_SIZE) < dsp->length)
	{
		__as_dsp_dtmf_detect(&(dsp->state) , dsp->line_samples+dsp->bufp, DTMF_DETECT_STEP_SIZE, 1);
		dsp->bufp += DTMF_DETECT_STEP_SIZE;
	}

	res = __as_dsp_dtmf_get(&(dsp->state), callerId, max);

	return res;
}

/* sample_length is measured by 'unsiged short' */
assist_dtmf_detector_t *assist_dsp_dtmf_detect_init( as_law_type_t law, int sample_length)
{
	assist_dtmf_detector_t  *dsp;

	dsp = (assist_dtmf_detector_t  *)malloc(sizeof(assist_dtmf_detector_t) );
	if(!dsp)
	{
		printf("No critical memory %s", __FUNCTION__);
		exit(1);
	}
	memset(dsp, 0, sizeof(assist_dtmf_detector_t) );
	dsp->law = law;
	dsp->length = sample_length;

	dsp->line_samples = (unsigned short *)malloc(sizeof(unsigned short)*sample_length );
	if(!dsp)
	{
		printf("No critical memory %s", __FUNCTION__);
		exit(1);
	}
	memset(dsp->line_samples, 0, sizeof(unsigned short)*sample_length );

	__as_dsp_dtmf_detect_init(dsp);

	return dsp;
}

void assist_dsp_dtmf_detect_detsory(assist_dtmf_detector_t  *dsp)
{

	if(dsp->line_samples)
	{
		free(dsp->line_samples);
		dsp->line_samples = NULL;
	}

	free(dsp);
	dsp = NULL;
}

/* return number of callerID char */
int  assist_dsp_dtmf_detect( int fd, int law, int length, char *callerId, int max)
{
	as_dtmf_detector_t *dsp;
	int res;
	
	dsp = as_dsp_init_dtmf_detect( law, length);

	res = _as_dsp_dtmf_detect(dsp, fd, callerId, max);
	
	as_dsp_detsory_dtmf_detect( dsp);

	return res;
}

/* return value : number of callerID char */
int  as_dsp_dtmf_detect_with_buffer(assist_dtmf_detector_t *dsp,  char *buf, int length, char *callerId, int max)
{
	;
	int res;
	
	dsp = as_dsp_init_dtmf_detect( law, length);

	res = _as_dsp_dtmf_detect_with_buffer(dsp, buf, length, callerId, max);
	
	as_dsp_detsory_dtmf_detect( dsp);

	return res;
}


