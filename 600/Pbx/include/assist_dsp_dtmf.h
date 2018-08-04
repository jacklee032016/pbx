#ifndef  __ASSIST_DSP_DTMF_H__
#define __ASSIST_DSP_DTMF_H__

/* keep the calculate result of every Goertzel step */
typedef struct
{
	float v2;
	float v3;
	float fac;
}as_goertzel_state_t;

#define	AS_MAX_DTMF_DIGITS 	128

typedef struct
{
	int hit1;
	int hit2;
	int hit3;
	int hit4;

	int mhit; /* MF ?*/

	as_goertzel_state_t row_out[4];
	as_goertzel_state_t col_out[4];
	as_goertzel_state_t row_out2nd[4];
	as_goertzel_state_t col_out2nd[4];
	as_goertzel_state_t fax_tone;
	as_goertzel_state_t fax_tone2nd;
	float energy;
    
	int current_sample;
	char digits[AS_MAX_DTMF_DIGITS + 1];
	int current_digits;
	int detected_digits;
	int lost_digits;
	int digit_hits[16];
	int fax_hits;
}as_dtmf_detect_state_t;

typedef struct 
{

	as_dtmf_detect_state_t  state;

	unsigned short  *line_samples;		/* buffer of linear sample data */
	int bufp;							/* buffer position*/
	int length;						/* length of buffer in 16 bit */

	as_law_type_t law;
	
}assist_dtmf_detector_t;


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

#endif

