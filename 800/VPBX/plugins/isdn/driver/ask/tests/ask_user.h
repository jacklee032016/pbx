/*
$Id: ask_user.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef  __ASK_USER_H__
#define __ASK_USER_H__

#define FLG_SEND_TONE				0x0001
#define FLG_SEND_DATA				0x0002
#define FLG_BCHANNEL_SETUP		0x0010
#define FLG_BCHANNEL_DOACTIVE		0x0020
#define FLG_BCHANNEL_ACTIVE		0x0040
#define FLG_BCHANNEL_ACTDELAYED	0x0080
#define FLG_CALL_ORGINATE			0x0100
#define FLG_BCHANNEL_EARLY			0x0200

#define MAX_REC_BUF			4000
#define MAX_DATA_BUF			1024

#define IE_BEARER_TYPE_64K			0		/* normal 64k circuit switch mode */
#define IE_BEARER_TYPE_AUDIO		1		/* audio(64k circuit switch mode) */

#define TE_RX_TX_VOICE			0
#define TE_RX_TOUCH_TONE		1
#define TE_TX_TOUCH_TONE		2
#define TE_RX_TX_HDLC_DATA	3
#define TE_RX_TX_X75			4
#define TE_RX_TX_VOICE_B_E		5		/* voice early B connection */


#define	MsgHead(ptr, cref, mty) \
	*ptr++ = 0x8; \
	if (cref == -1) { \
		*ptr++ = 0x0; \
	} else { \
		*ptr++ = 0x1; \
		*ptr++ = cref^0x80; \
	} \
	*ptr++ = mty

void usage(char *pname, char *usage_msg) ;

#endif

