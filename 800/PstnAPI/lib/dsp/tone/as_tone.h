/*
 * $Log: as_tone.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.4  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.3  2006/05/28 10:14:58  lizhijie
 * dd
 *
 * Revision 1.2  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.1  2006/03/20 06:37:26  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/13 02:50:20  lizhijie
 * add tone defination for IIT,SDT,NFT,CPT and play mechanism for tone
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_tone.h,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/


#ifndef  __AS_DSP_TONE_H__
#define __AS_DSP_TONE_H__

#include "assist_lib.h"

typedef	struct 	tone_data		tone_data_t;
typedef	struct	audio_tone		audio_t;
typedef	struct	tone_dsp			tone_t;


struct tone_data
{
	unsigned char		*mark;		/* audio data of 'length' msg 	*/
	unsigned char		*spare;		/* silence data of 'length' ms 	*/
	int 				length;		/* unit in ms */
};


struct audio_tone
{
	law_t			law;
	int				duration;		/* unit in ms */
	int				mark;			/* unit in ms */

	int 				readPosition;	/* current read position */
	int				index;

	float				freq1;
	float				freq2;

	tone_data_t		*data;

	struct tone_dsp	*dsp;
};

typedef struct
{
	audio_t			tone;
	
	unsigned char		*beginPart;
	int				length;			/* length of begin part in ms */
}iit_tone_t;

struct tone_dsp
{
	int				law;
	int				length;			/* unit in ms, 8 bytes/ms, return length of every read operation, must < 100ms for DTMF duration is 100ms  */
	float				freq1;			/* compliant with DTMF */
	float				freq2;

	audio_t			dialTone;		/* DT */
	audio_t			ringBack;		/* RBT , refer specs, p.21 */
	audio_t			busyTone;		/* BT */

	audio_t			secondDT;		/* Second Dial Tone P.22 */

	audio_t			notifyTone;		/* Notification Tone(NFT), alias as CPT(Acceptance Tone) */

	iit_tone_t			iitTone;
	
	audio_t			dtmf_chars[16];

	tone_data_t		*data;			/* common tone data for all tones */
	
	as_dsp_t			*root;
};

int as_dsp_tone_arbtones(unsigned char *data , float f0 , float f1 , int len , int law );

/* interface exposed to dsp.c */
extern	int 		as_tone_read(audio_t *tone,  char *buffer);

extern	tone_t 	*as_dsp_new_tones(as_dsp_t *dsp);
extern	void 	as_dsp_destroy_tone(tone_t	*dsp);

/* read tone data for DT, BT, RBT, SDT */
extern	int 		as_tone_read_DT(audio_t *audio,  char *buffer);

#endif

