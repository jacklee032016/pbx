/*
* $Id: macros.h,v 1.11 2007/07/25 17:48:34 lizhijie Exp $
*/
#ifndef	__MACRO_H__
#define	__MACRO_H__

#define	TEST_NVRAM				0
#define	UART_DEBUG				1

#define	WITH_OPTIMIZE_SIZE		1

#define	JANPAN			0
#define	DIAL_X			0
#define	SELF_CHECK		0

#define	WITH_IP_VOICE_CHANNEL		0

#if		JANPAN
#define		RINGBACK_LOOP_COUNT			27		/* 13*TONE_TIME_RINGBACK_ON+14*TONE_TIME_RINGBACK_OFF */
#define		TONE_TIME_RINGBACK_OFF		20		/* 20*0.1s =2000ms */
#define		TONE_TIME_RINGBACK_ON		10		/*10*0.1s = 1000ms */
#define		busysound_time					30		//30*busysound_space
#define		busysound_space				5		//5*0.1 s
#else

#define		RINGBACK_LOOP_COUNT			16		/* in unit of RINGBACK_ON+ RINGBACK_OFF */
#define		TONE_TIME_RINGBACK_ON		10		/* 40*0.1 s */
#define		TONE_TIME_RINGBACK_OFF		40		/* 40*0.1 s */
#define		busysound_time					50		/* 50*busysound_space	*/
#define		busysound_space				3		/* 3*0.1 s */
#endif

#define		EVENT_FLASH_TIME				14*PBX_50MS	/* 1500 ms */
#define		HOLD_SERVICE_TONE_TIME		5*PBX_100MS		/* 500ms space and mark, NTT specs, p.22 */
#define		HST_DURATION					8				/* 8*500ms = 4 seconds period */
#define		HST_TOTAL_LENGTH				80				/* 80*500ms = 40 seconds */

#define		ip_backsound_time	55		//27*TONE_TIME_RINGBACK_ON+28*TONE_TIME_RINGBACK_ON
#define		bohao_time_ip		1000		//打IP的拨号时间1000*0.1
#define		cuigua_time		7		//7*cgy_space
#define		ldts_space		3		//3*0.1s	来电提示时间
#define		ldts_time		3		//3*ldts_space
#define		ldts_next_time		40		//41*0.1s
#define		backsound_time10	40		//40*1s
#define		track_zj_time		46		//46*0.1s	中继模拟摘机时间
#define		yuyin_time		100		//100*0.1 s     放音时间
#define		busysound_time1		5		//摘机出外线不成功时 5*busysound_space
#define		paicha_time		7		//拍叉时间
#define		meeting_busysound_time	10

#define		cgy_space		1		//1*0.1 s催挂音间隔
#define		mask_track_time		2		//2*0.1s

#define		RECORD_TIMER_LENGTH		100*PBX_100MS
#define		DIALING_TIMER_LENGTH		200		/* timeout length of dialing, about 200*0.1 s */

#define		NO_RESPONSE_TIMEOUT		600	/* 1 minuts */
#define		TIMEOUT_NEVER				0

#define		iic_id0			20		/*EEROM出厂配置数据*/
#define		iic_id1			07
#define		iic_id2			10
#define		iic_id3			14

/*
* FM24c08 EEPROM hardware parameters : 
*     8192 bits = 1024 bytes
*     16 bytes/page, total 64 Pages
*     10 bits address and 4 bits in a page
*/
#define		FM24C08_DEVICE_ADDRESS		0XA0

#define		FM24C08_READ_CMD				TRUE
#define		FM24C08_WRITE_CMD			FALSE

#define		FM2408_PAGE_SIZE				16


#define	NVRAM_PAGE_OFFSET_GLOBAL			0		/* 8 pages = 128 bytes */
#define	NVRAM_PAGE_OFFSET_EXT				8		/* 10 page = 160 bytes */
#define	NVRAM_PAGE_OFFSET_NIGHT				18		/* 12 page = 192 bytes */
#define	NVRAM_PAGE_OFFSET_UNCONDITIONAL	30		/* 12 page = 192 bytes */		
#define	NVRAM_PAGE_OFFSET_LEFT				42		/* 22 page = 352 bytes */


#include		<REGSTCRD+.H>
#include		<intrins.h>
#define	NOP()					_nop_()				

#define	CPLD_CMD_HT1381_CTRL				0xF0	/* #11110000B */
#define	CPLD_CMD_TONE_CTRL				0xF1	/* #11110001B */
#define	CPLD_CMD_COME_CTRL				0xF2	/* #11110010B */
#define	CPLD_CMD_DTMF0_ENABLE			0xF5	/* #11110101B */
#define	CPLD_CMD_DTMF0_DISABLE			0XFD 	/* #11111101B */
#define	CPLD_CMD_DTMF1_ENABLE			0xF6	/* #11110110B */
#define	CPLD_CMD_DTMF1_DISABLE			0xFE	/* #11111110B */

#define	CPLD_CMD_EXT_HOOK_CONTROL		0xF7	/* #11110111B */
#define	CPLD_CMD_PSTN_HOOK_CONTROL		0xF8	/* #11111000B */

#define	CPLD_CMD_RECORDER_PLAY_START	0xF9	/* #11111001B */
#define	CPLD_CMD_RECORDER_PLAY_STOP		0xFA	/* #11111010B */
#define	CPLD_CMD_RECORDER_REC_START		0XFB	/* #11111011B */
#define	CPLD_CMD_RECORDER_REC_STOP		0XFC	/* #11111100B */

#define	CPLD_CMD_DISABLE_ALL				0XFF	/* #11111100B */

#endif

