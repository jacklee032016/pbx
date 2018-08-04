/*
 * $Log: as_dtmf_decoder.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.2  2006/05/30 03:06:46  lizhijie
 * add newly created files for optimized DTMF algorithm
 *
 * Revision 1.1  2006/05/30 03:05:04  lizhijie
 * no message
 *
 * $Id: as_dtmf_decoder.h,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#ifndef  __AS_DTMF_DECODER_H__
#define __AS_DTMF_DECODER_H__
/* 
* tone_detect.h - General telephony tone detection, and specific detection of DTMF.
*/

#include "assist_lib.h"

/* keep the calculate result of every Goertzel step */
typedef struct
{
	float 					v2;
	float 					v3;
	float 					fac;
}as_gstate_t;

#define	AS_MAX_DTMF_DIGITS 				128

typedef enum
{
	AS_DTMF_STAGE_SPARE = 0,
	AS_DTMF_STAGE_MARK,
}dtmf_stage_t;


typedef struct
{
	int 						hit1;
	int 						hit2;
	int 						hit3;
	int 						hit4;

#if 0
	int 						mhit; /* MF ?*/
#endif

	as_gstate_t 				row_out[4];
	as_gstate_t 				col_out[4];

	float 					energy;
    
	dtmf_stage_t				lastStage;
	dtmf_stage_t				currentStage;
	int						ignoreLength;
	
	char 					digits[AS_MAX_DTMF_DIGITS + 1];
	int 						current_digits;
	
	int 						detected_digit;
}as_dtmf_state_t;

/* keep the coeffiecent for every frequency element , this is not variable in algorithm */
typedef struct
{
	float 					fac;
} as_dtmf_descriptor_t;

typedef struct
{
	int						law;

	int						sizeofBuffer;
	short					*buf;
	int 						bufp;

	as_dtmf_state_t			state;
	
	as_dsp_t					*root;
}as_dtmf_decoder_t;


/* interface exposed to dsp.c */
extern	as_dtmf_decoder_t*	as_dtmf_new_decoder(as_dsp_t *root);
extern	void 	as_dtmf_destroy_decoder(as_dtmf_decoder_t *dsp);
extern	int 		as_dtmf_decoding(as_dtmf_decoder_t *dsp, unsigned char *sample, int length, char *callId, int max);
extern	void 	as_dtmf_reset_decoder(as_dtmf_decoder_t *dsp);

#endif

