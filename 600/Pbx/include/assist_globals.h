#ifndef  __ASSIST_GLOBAL_H__
#define  __ASSIST_GLOBAL_H__
/* 
 *$Id: assist_globals.h,v 1.2 2007/09/08 18:21:40 lizhijie Exp $
 * Here are all defination about assist device drivers, 
 * All these interfaces are used in C environments
*/ 

#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "asstel.h"
#include "as_fxs.h"

#include "as_dev_ctl.h"

#include "common_globals.h"
#include "pbx_globals.h"

#define AS_EVENT_DEBUG			1

/* from and for as_lib_dtmf_generator.c */
#define	AS_MAXDTMF 64
#define	AS_DTMF_BLOCKSIZE 204	/* perfect for goertzel */
#define	AS_DEFAULT_DTMFLENGTH 	100 /* default tone length -- 100 ms */

#if 0
/* from and for as_dsp_law.c */
unsigned char   as_lib_law_linear2alaw (short linear);
unsigned char  as_lib_law_linear2ulaw(short sample);
short as_lib_law_ulaw2linear(unsigned char sample);
short as_lib_law_alaw2linear(unsigned char sample);
short as_lib_law_full_ulaw2linear(unsigned char sample);
short as_lib_law_full_alaw2linear(unsigned char sample);

#define	LINEAR2XLAW(s,law) ((law == U_LAW_CODE) ? as_lib_law_linear2ulaw(s) : as_lib_law_linear2alaw(s)) 
#define	XLAW(c,law) ((law ==  U_LAW_CODE) ? as_lib_law_ulaw2linear(c): as_lib_law_alaw2linear(c) )
//#define	FULLXLAW(c,law) ((law ==  U_LAW_CODE) ? as_lib_law_full_ulaw2linear(c): as_lib_law_full_alaw2linear(c))
#define	FULLXLAW(c,law) ((law ==  U_LAW_CODE) ? as_lib_law_full_ulaw2linear(c): as_lib_law_full_alaw2linear(c))
#endif

int as_lib_event_get(int fd);


/*added by lijie 2005-02-25*/
#define AS_DIALING_MAX_NUM 16

typedef struct
{	
	int iLen;
	char buf[AS_DIALING_MAX_NUM];		
}TelCode;

typedef enum
{/*cmd number*/
	AS_PBX_CMD_TYPE_CALLER_OFFHOOK 				= 0x91, 	/* PBX caller offhook */
	AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT 			= 0x92,	/* PBX send caller number to IP */
	AS_PBX_CMD_TYPE_CALLER_ONHOOK 				= 0x93, 	/* PBX onhook first when in-call */
	AS_PBX_CMD_TYPE_PCM_RESET					= 0x94,					

	AS_PBX_CMD_TYPE_CALLEE_OFFHOOK 				= 0x85,	/* PBX offhook as a callee */
	AS_PBX_CMD_TYPE_CALLEE_SEND_PCM_CHANNEL 	= 0x86,
	AS_PBX_CMD_TYPE_CALLEE_BUSY_HERE 			= 0x87
}AS_PBX_CMD_TYPE;

#if 0
/* command send from IP to PBX */
#define QUERY_CHANNEL_CMD 	0x01
#define START_TONE_CMD			0x04   	/* not used , commented now , 2005.03.01  */
#define DIAL_VALID_CMD			0x11		/* not used , 2005.03.01 */
#define IP_TRANSFER_CMD		0x17
#endif


#define DIAL_CODE_CMD			0x06		/* transfer IP caller phone number, such as IP address */
#define START_RING_CMD 		0x03	
#define IP_RESET_CMD			0x04

//#define DIAL_INVALID_CMD		0x12	/* dialing number is not validate */
#define IP_BUSY_CMD				0x13		/* used to note PBX play busy tone one branchs */
#define IP_IDLE_CMD				0x14 	 /* command which note PBX to ringback */
#define OPEN_CHANEL_CMD 		0x15  	/* used to note PBX stop ringback and open channel, it also used to stop ring */	
#define IP_ONHOOK_CMD			0x16  	/* used to stop ring on PBX. but now it is replaced by 0x15 */
#define IP_CANCEL_REQUEST		0x17		/* used to response to the SIP CANCEL request ,eg. when PBX is ringing, the call is cancelled by caller, then stop PBX ringing*/


#include "pbx_globals.h"
#include "aslib.h"

typedef struct 
{
	int			iEnableDial;
	int 			chan;
	as_state_t 	state; 
	as_state_t	logState;
	TelCode 		telCode;
}CHANNEL_INFO;

typedef struct
{
	int			iEnablePstn;
	as_device_t 	*dev;
	char 		pstnBuf[AS_DIALING_MAX_NUM];
	as_state_t 	pstnState;
}PSTN_INFO;

#endif

