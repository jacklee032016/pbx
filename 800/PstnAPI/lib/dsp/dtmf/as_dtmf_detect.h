/*
 * $Log: as_dtmf_detect.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.5  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.4  2006/05/28 10:14:58  lizhijie
 * dd
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
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_dtmf_detect.h,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#ifndef  __AS_DTMF_DETECT_H__
#define __AS_DTMF_DETECT_H__
/* 
* tone_detect.h - General telephony tone detection, and specific detection of DTMF.
*/

#include "assist_lib.h"

#define WITH_FAX_DETECT			0
#define WITH_2ND_HARMONIC			0

/* keep the calculate result of every Goertzel step */
typedef struct
{
	float 					v2;
	float 					v3;
	float 					fac;
}as_goertzel_state_t;

#define	AS_MAX_DTMF_DIGITS 	128

typedef struct
{
	int 						hit1;
	int 						hit2;
	int 						hit3;
	int 						hit4;

#if 0
	int 						mhit; /* MF ?*/
#endif

	as_goertzel_state_t 		row_out[4];
	as_goertzel_state_t 		col_out[4];
#if WITH_2ND_HARMONIC
	as_goertzel_state_t 		row_out2nd[4];
	as_goertzel_state_t 		col_out2nd[4];
#endif

#if WITH_FAX_DETECT
	as_goertzel_state_t 		fax_tone;
	as_goertzel_state_t 		fax_tone2nd;
#endif

	float 					energy;
    
	int 						current_sample;
	char 					digits[AS_MAX_DTMF_DIGITS + 1];
	int 						current_digits;
	int 						detected_digits;
	int 						lost_digits;
	int 						digit_hits[16];
#if WITH_FAX_DETECT
	int 						fax_hits;
#endif
}as_dtmf_detect_state_t;

/* keep the coeffiecent for every frequency element , this is not variable in algorithm */
typedef struct
{
	float 					fac;
} as_tone_detection_descriptor_t;

#define DTMF_RAWDATA_LENGTH_4_DIGIT   				160

typedef struct
{
	int						law;

	int						length;
	short					*buf;
	int 						bufp;

	int						lengthOfDigit;			/* raw(u/a law) data length for a DTMF digit */
	int						position;				/**/

	as_dtmf_detect_state_t		state;
	
	as_dsp_t					*root;
}dtmf_t;


/* interface exposed to dsp.c */
extern	dtmf_t *	as_dsp_new_dtmf(as_dsp_t *root);
extern	void 	as_dsp_destroy_dtmf(dtmf_t *dsp);
extern	int 		as_dtmf_detect(dtmf_t *dsp, unsigned char *sample, int length, char *callId, int max);

#endif

