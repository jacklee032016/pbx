/*
 * $Log: as_busy_detect.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.8  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.7  2006/05/28 10:14:58  lizhijie
 * dd
 *
 * Revision 1.6  2006/04/19 12:28:39  lizhijie
 * add CAR support and others
 *
 * Revision 1.5  2006/04/19 12:24:10  lizhijie
 * no message
 *
 * Revision 1.4  2006/04/06 02:38:36  wangwei
 * no message
 *
 * Revision 1.3  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.2  2006/03/20 06:35:29  lizhijie
 * no message
 *
 * Revision 1.1  2006/03/17 07:56:25  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_busy_detect.h,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#ifndef __AS_BUSY_DETECT_H__
#define __AS_BUSY_DETECT_H__

#include "assist_lib.h"

#define AS_BUSY_DETECT_DEBUG					0


#define AS_DSP_FEATURE_SILENCE_SUPPRESS 		(1 << 0)
#define AS_DSP_FEATURE_BUSY_DETECT      		(1 << 1)

#define AS_BUSY_DEFAULT_THRESHOLD 			40000 //1024	/* energy level threshold between SILENCE and NOISE in linear mode */
/*wangwei , 2006.04.05*/
//#define AS_BUSY_DEFAULT_THRESHOLD 			30000 //1024	/* energy level threshold between SILENCE and NOISE in linear mode */

#define AS_BUSY_PERCENT						10	/* The percentage difference between the two last silence periods */
#define AS_BUSY_THRESHOLD						100	/* Max number of ms difference between max and min times in busy */
#define AS_BUSY_MIN								75	/* Busy must be at least 80 ms in half-cadence */
#define AS_BUSY_MAX							1100	/* Busy can't be longer than 1100 ms in half-cadence */

/* Remember last 15 units */


/* Define if you want the fax detector -- NOT RECOMMENDED IN -STABLE */
#define AS_FAX_DETECT
#define AS_TONE_THRESH 						10.0	/* How much louder the tone should be than channel energy */
#define AS_COUNT_THRESH  						3		/* Need at least 50ms of stuff to count it */


/* cadence count of busy tone , one cadence is about n*1000ms(500ms+500ms) */
#define 	AS_BUSY_CADENCE_THRESHOLD				4

#define AS_BUSY_HISTORY 							5		/* number of BT cadence loop */

typedef struct 
{
//	law_t 			law;
	int	 			law;
	int 				features;
	
	int 				energyThreshold;					/* energy threshold between SILENCE and NOISE */
	
	unsigned short 	*buf;								/* length is short for compatibel for our driver */
	int				length;								/* length of this buffer */
	int				bufp;
	
	int 				totalSilence;							/* in the unit of ms in current duration */
	int 				totalNoise;							/* in the unit of ms in current duration */
	
	int 				historicNoise[AS_BUSY_HISTORY];
	int 				historicSilence[AS_BUSY_HISTORY];

	int 				busyCount;					/* SILENCE<-->NOISE switch number */

	int 				busyMaybe;
	
	as_dsp_t			*root;
}busy_t ;

/* interface exposed to dsp.c */
extern	busy_t 	*as_dsp_new_busydetect(as_dsp_t *root);
extern	void 	as_dsp_destroy_busydetect(busy_t *dsp);
extern	int 		as_busydetect(busy_t *dsp, unsigned char *buff,int length);

#endif

