/*
* $Id: pbxHw.h,v 1.1 2007/07/30 07:12:21 lizhijie Exp $
*/
#ifndef	__PBX_HW_H__
#define	__PBX_HW_H__

/******************************************
 Following are 74HC574 which is CS by EPM7064 and must maintain status info for these chip
*******************************************/
/* U25, EXT relays , schema 3.D */

/* U27 and U28, PSTN line relays and following pins, refer to schema 3.D */
#define	PSTN_RELAY_CTRL_RESET_8816			4
#define	PSTN_RELAY_CTRL_BUZZER				5

/* U30, tone ctrl for EXTs, refer to 3.A 
  high level to CD4066 Ctrl pins*/

/* U31. refer to schema 3.A 
* signal of Come_C0~C3 and WDI(IMP813),EPMRST,PDOWN(8823AP power down) is select by U31
*/
#define	PSTN_FSK_CTRL_WDI						4		/* WDI pin of IMP810, eg. watchdog */
#define	PSTN_FSK_CTRL_EPM_RST				5		/* reset to EPM7064 */
#define	PSTN_FSK_CTRL_P_DOWN					6		/*Power Down pin to PDOWN of SM8823AP */

/* U38, provide digits to 2 DTMF encoders BT9200B, refer to 4.B */


typedef	enum
{
	HW_STATUS_EXT_HOOK,
	HW_STATUS_PSTN_HOOK_BUSY,
	HW_STATUS_DTMF_DECODER,
	HW_STATUS_OTHERS,
}HW_STATUS_T;


/***************************************
 Following are pins definations 
****************************************/
#define	FSKDATA								WR
#define	FSKCK									RD
#define	HC138IN								P2
#define	MT88E39_DCLK							RD		/* RD of STC */
#define	MT88E39_DATA							P0_3	//WR	/* WR of STC */
#define	MT88E39_DATA_ENABLE					T1		/* enable U58:74HC245  */


#define	DTMF_ENCODER_CS						P1_0	/* CLK for 74HC574 (U38) which enable DTMF digits is send out from P0 to HT9200B  */
#define	MT8816_CS								P1_1	/* to DM7407M, U33 */
#define	MT8816_ST0								P1_2
#define	MT8816_ST1								P1_3
#define	CTS										P1_4
#define	HT1381_SCLK							P1_5
#define	HT1381_DATA							P1_7
#define	FM2408_SCLK							P1_6
#define	FM2408_SDATA							P1_7


/* following pins connect to EPM7064 */
#define	HC138A									P2_0
#define	HC138B									P2_1
#define	HC138C									P2_2
#define	HC138D									P2_3
#define	EPM7064_CS								P2_4

/* following pins are connect to 74HC245 for device status access */
#define	READ_EXT_HOOK_CS						P2_5
#define	READ_PSTN_HOOK_CS					P2_6
#define	READ_DTMF_CS							P2_7
#define	READ_OTHERS_CS						T1		/* enable U58:74HC245  */

/* enable music and ringback of ext and 8 DTMF ecoder(LC375) , refer to 6.C and 1.D */
#define	HC138EN								T0

#define	HC138_CS_DTMF_DECODER_01			0x00
#define	HC138_CS_DTMF_DECODER_23			0x01
#define	HC138_CS_DTMF_DECODER_45			0x02
#define	HC138_CS_DTMF_DECODER_67			0x03
#define	HC138_CS_RINGBACK_TONE				0x04
#define	HC138_CS_MUSIC						0x05


#define		PBX_FXS_COUNT						8
#define		PBX_FXO_COUNT						4
#define		PBX_IP_COUNT						4

#define		PBX_DEVICE_COUNT					(PBX_FXS_COUNT+PBX_FXO_COUNT+PBX_IP_COUNT)
#define		PBX_ROW_DEVICE_COUNT				(PBX_FXS_COUNT+PBX_FXO_COUNT)

#define		ENCODE_CHANNEL_NUMBER			4		/* only 2 HW encoding channel and 4 SW encoding channel */
#define		DECODE_CHANNEL_NUMBER			4		/* 8 DTMF decoder(LC7385), first 4 is connect to array, last 4 connecto EXT0~3 */

/* Array Columns defination */
/* raw0~row7 is for 8 Extensions; row9~row11 is for 4 PSTN Lines */
#define		CHANNEL_ROW_DTMF_ENCODE_1		12
#define		CHANNEL_ROW_DTMF_ENCODE_2		13
#define		CHANNEL_ROW_IP_R_1				14		/* used to play tone or music on IP channel */
#define		CHANNEL_ROW_IP_R_2				15		/* used to decode phone number on IP channel */

#define		CHANNEL_COL_OFFSET				0
/* Array Columns defination */
#define		CHANNEL_COL_DTMF_DECODE_1		(8+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_DTMF_DECODE_2		(9+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_DTMF_DECODE_3		(10+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_DTMF_DECODE_4		(11+CHANNEL_COL_OFFSET)

#define		CHANNEL_COL_RINGBACK_TONE		(12+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_MUSIC				(13+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_RECORD				(14+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_TONE					(15+CHANNEL_COL_OFFSET)

#include		<REGSTCRD+.H>
#include		<intrins.h>
#define		NOP()								_nop_()				

#define	CPLD_CMD_HT1381_CTRL					0xF0	/* #11110000B */
#define	CPLD_CMD_TONE_CTRL					0xF1	/* #11110001B */
#define	CPLD_CMD_COME_CTRL					0xF2	/* #11110010B */
#define	CPLD_CMD_DTMF0_ENABLE				0xF5	/* #11110101B */
#define	CPLD_CMD_DTMF0_DISABLE				0XFD 	/* #11111101B */
#define	CPLD_CMD_DTMF1_ENABLE				0xF6	/* #11110110B */
#define	CPLD_CMD_DTMF1_DISABLE				0xFE	/* #11111110B */

#define	CPLD_CMD_EXT_HOOK_CONTROL			0xF7	/* #11110111B */
#define	CPLD_CMD_PSTN_HOOK_CONTROL			0xF8	/* #11111000B */

#define	CPLD_CMD_RECORDER_PLAY_START		0xF9	/* #11111001B */
#define	CPLD_CMD_RECORDER_PLAY_STOP			0xFA	/* #11111010B */
#define	CPLD_CMD_RECORDER_REC_START			0XFB	/* #11111011B */
#define	CPLD_CMD_RECORDER_REC_STOP			0XFC	/* #11111100B */

#define	CPLD_CMD_DISABLE_ALL					0XFF	/* #11111100B */


/****************************************************
      Following are hw control macros 
*****************************************************/
#define	RECORDER_REC_START()	\
	cfg_cpld_cmd(CPLD_CMD_RECORDER_REC_START)

#define	RECORDER_REC_STOP()	\
	cfg_cpld_cmd(CPLD_CMD_RECORDER_REC_STOP)

#define	RECORDER_PLAY_START()	\
	cfg_cpld_cmd(CPLD_CMD_RECORDER_PLAY_START)

#define	RECORDER_PLAY_STOP()	\
	cfg_cpld_cmd(CPLD_CMD_RECORDER_PLAY_STOP)

#define	RECORDER_RESET()	\
	do{RECORDER_PLAY_STOP();	\
		RECORDER_REC_STOP();}while(0)	

#define	DTMT_ENCODER0_DISBALE()	\
	cfg_cpld_cmd(CPLD_CMD_DTMF0_DISABLE)

#define	DTMT_ENCODER1_DISBALE()	\
	cfg_cpld_cmd(CPLD_CMD_DTMF1_DISABLE)

#define	HC138_DISABLE() \
	(HC138EN = FALSE)

#define	DTMF_DECODER_DISABLE() \
	HC138_DISABLE()

/* enable HC138 pin 0~3 */
#define	DTMF_DECODER_ENABLE(index) \
	hc138_enable((index))


#define	EXT_CTRL_RINGING()	\
	cfg_cpld_value_cmd(CPLD_CMD_EXT_HOOK_CONTROL, extDevices.relayCtrl)

#define	EXT_CTRL_TONE()	\
	cfg_cpld_value_cmd(CPLD_CMD_TONE_CTRL, 		extDevices.toneCtrl)

#define	EXT_CTRL_MUSIC()	\
	hc138_value_cmd(HC138_CS_MUSIC, 				extDevices.musicStatus)

#define	EXT_CTRL_RINGBACK()	\
	hc138_value_cmd(HC138_CS_RINGBACK_TONE, 		extDevices.ringbackStatus)

#define	PSTN_CTRL_HOOK()	\
	cfg_cpld_value_cmd(CPLD_CMD_PSTN_HOOK_CONTROL, pstnDevices.relayCtrl)

#define	PSTN_CTRL_DECODER()	\
	cfg_cpld_value_cmd(CPLD_CMD_COME_CTRL, 		pstnDevices.fskCtrl)

#define	WATCHDOG_FEED()	\
	do{pstnDevices.fskCtrl = pstnDevices.fskCtrl ^SET_BIT(1, PSTN_FSK_CTRL_WDI);\
		PSTN_CTRL_DECODER();}while(0)

#define	EPM7064_RESET()	\
	do{	PBX_SET_FLAGS(pstnDevices.fskCtrl, PSTN_FSK_CTRL_EPM_RST);\
	PSTN_CTRL_DECODER();NOP();NOP();NOP();\
	PBX_CLEAR_FLAGS(pstnDevices.fskCtrl, PSTN_FSK_CTRL_EPM_RST);\
	PSTN_CTRL_DECODER();}while(0)

#define	CONNECT(row, col) \
	switch_connect(row, col, TRUE)
	
#define	DISCONNECT(row, col) \
	switch_connect(row, col, FALSE)

#define	RESET_PBX()	\
	(ISP_CONTR = CMD_CFG_IP_PBX_RESET_ISP)

#define		HT1381_RESET_ON()	\
	do{HT1381_SCLK= FALSE; P2 = CPLD_CMD_HT1381_CTRL;P2 = CPLD_CMD_HT1381_CTRL;P2 = CPLD_CMD_HT1381_CTRL;	\
		NOP();NOP();\
		EPM7064_CS = FALSE;}while(0)

#define		HT1381_RESET_OFF()	\
	do{P2 = CPLD_CMD_DISABLE_ALL;P2 = CPLD_CMD_DISABLE_ALL;P2 = CPLD_CMD_DISABLE_ALL;	\
		NOP();NOP();\
		EPM7064_CS = FALSE;HT1381_SCLK= FALSE; }while(0)

/* Hardware Info of FM2408 */
/*
* FM24c08 EEPROM hardware parameters : 
*     8192 bits = 1024 bytes
*     16 bytes/page, total 64 Pages
*     10 bits address and 4 bits in a page
*/
#define		FM24C08_DEVICE_ADDRESS			0XA0

#define		FM24C08_READ_CMD					TRUE
#define		FM24C08_WRITE_CMD				FALSE

#define		FM2408_PAGE_SIZE					16


#define	NVRAM_PAGE_OFFSET_GLOBAL			0		/* 8 pages = 128 bytes */
#define	NVRAM_PAGE_OFFSET_EXT				8		/* 10 page = 160 bytes */
#define	NVRAM_PAGE_OFFSET_NIGHT				18		/* 12 page = 192 bytes */
#define	NVRAM_PAGE_OFFSET_UNCONDITIONAL	30		/* 12 page = 192 bytes */		
#define	NVRAM_PAGE_OFFSET_LEFT				42		/* 22 page = 352 bytes */

	
#endif

