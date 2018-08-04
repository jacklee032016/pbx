/*
* $Id: pbx_cmd.h,v 1.1 2007/03/11 10:32:12 lizhijie Exp $
*/

#ifndef __PBX_CMD_H__
#define __PBX_CMD_H__

/* how to process the case of no dial after dial in */
#define	PBX_CMD_NO_DIAL_ACTION_CONSOLE		0xa4	/* auto to console */
#define	PBX_CMD_NO_DIAL_ACTION_BUSY			0xa5	/* broken */

/* line to line : */
#define	PBX_CMD_LINE_2_LINE_DISABLE			0xa6
#define	PBX_CMD_LINE_2_LINE_ENABLE			0xa7

/* ring differently for phone and lines */
#define	PBX_CMD_DIFFER_RING_OFF				0xa8
#define	PBX_CMD_DIFFER_RING_ON				0xa9

/* DTMF is 0, FSK is 1 */
#define	PBX_CMD_CALLID_MODE_FSK				0xaa
#define	PBX_CMD_CALLID_MODE_DTMF			0xab

/* prefix of PSTN call : 0~9 . Why no parameters ? */
#define	PBX_CMD_PSTN_PREFIX_ONE_NUMBER		0xb1	/* one digit number */
#define	PBX_CMD_PSTN_PREFIX_NO				0xb2


#define	PBX_CMD_IP_PREFIX						0xd4

/* first for special phone number */
#define	PBX_CMD_SPECIAL_FIRST_ON				0xd2
#define	PBX_CMD_SPECIAL_FIRST_OFF			0xd3

/* 0 :enabled; 1 : disabled */
#define	PBX_CMD_GROUP_CALL_ON				0xda
#define	PBX_CMD_GROUP_CALL_OFF				0xdb

#define	PBX_CMD_RESET_PBX						0xbd


#endif

