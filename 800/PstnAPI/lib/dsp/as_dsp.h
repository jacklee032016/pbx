/*
 * $Log: as_dsp.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.10  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.9  2006/04/20 05:17:24  lizhijie
 * add Call Progress Tone Detect
 *
 * Revision 1.8  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.7  2006/03/20 06:35:18  lizhijie
 * no message
 *
 * Revision 1.6  2006/03/17 07:58:10  lizhijie
 * add busy tone detect and debug problem of DTMF detect
 *
 * Revision 1.5  2006/03/16 08:41:29  lizhijie
 * add FSK support and FSK test programs in IXP4xx platform
 *
 * Revision 1.4  2006/03/16 08:37:16  lizhijie
 * no message
 *
 * Revision 1.3  2006/03/13 02:50:20  lizhijie
 * add tone defination for IIT,SDT,NFT,CPT and play mechanism for tone
 *
 * Revision 1.2  2006/03/12 11:10:31  lizhijie
 * change DTMF caller ID parameter from 60/120 to 100/200(mark/duration)
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_dsp.h,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/


#ifndef __AS_DSP_H__
#define __AS_DSP_H__

#include <sys/types.h>

#ifdef  __cplusplus
	extern "C"
	{
#endif

#define AS_DSP_DEBUG		1

#define trace		printf("%s[%d]\r\n", __FUNCTION__, __LINE__);


#define	AS_DTMF_BLOCKSIZE 				204		/* perfect for goertzel */
#define 	DTMF_DETECT_STEP_SIZE			102


#include "g711.h"
#include "as_tone.h"
#include "as_busy_detect.h"
#include "as_progress_detect.h"
#include "as_dtmf_detect.h"
#include "as_dtmf_decoder.h"
#include "as_fsk.h"


#define	LINEAR2XLAW(s,law) ((law == U_LAW_CODE) ? linear2ulaw(s) : linear2alaw(s)) 
#define	XLAW(c,law) ((law ==  U_LAW_CODE) ? ulaw2linear(c): alaw2linear(c) )
//#define	FULLXLAW(c,law) ((law ==  U_LAW_CODE) ? full_ulaw2linear(c): as_lib_law_full_alaw2linear(c))

typedef	struct
{
	tone_t				*tones;			/* audiable and DTMF tone player */
	busy_t				*busy;			/* busy tone detect */
	progress_t			*progress;		/* Call progress detect*/
	dtmf_t				*dtmf;			/* DTMF decoder */

	as_dtmf_decoder_t		*dtmfDecoder;	/* optimized DTMF decoder for Console */
	fsk_t				*fsk;			/* FSK encoder/decoder */
}as_dsp_priv_t;



#ifdef __cplusplus
	};
#endif


#endif

