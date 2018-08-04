/*
* $Id: pbx_pstn.h,v 1.6 2007/07/21 16:09:01 lizhijie Exp $
*/
#ifndef	__PBX_PSTN_H__
#define	__PBX_PSTN_H__

#define	WITH_PSTN_FLOW_DEBUG			0
#define	WITH_PSTN_FSK_DATA_DEBUG		1	

#define	WITH_FSK_MT88E39					1

/* SM8223AP can be Interupt external INT0 with falling edge or low level, default is low level */
#define	SM8223AP_FALLING_EDGE_SAMPLE	0

#if WITH_PSTN_FLOW_DEBUG
enum
{
	PSTN_FLOW_BEGIN_RINGER_ON			=	0,
	PSTN_FLOW_BEGIN_CAR					=	1,
	PSTN_FLOW_RINGER_OFF					=	2,
	PSTN_FLOW_RINGER_ON					=	3,
	PSTN_FLOW_RINGER_TIMEOUT			=	4,
	PSTN_FLOW_BEGIN_CHINA_CALLER_ID	=	5,

	PSTN_FLOW_BEGIN_CALLER_ID			=	7,
};
#endif

#if WITH_PSTN_FSK_DATA_DEBUG
enum
{
		FSK_DATA_ERR_START_BIT		=	0x0A,
		FSK_DATA_ERR_STOP_BIT		=	0X0B,
		FSK_DATA_ERR_FORMAT			=	0X0C,
		FSK_DATA_ERR_LENGTH			=	0X0D,
		FSK_DATA_RAW_DATA			=	0X0E,
};
#endif

#define	FSK_FORMAT_SDMF						0x04
#define	FSK_FORMAT_MDMF						0x80

/* month, day, hour, minute, every has 2 bytes */
#define	FSK_CHINESE_DATE_PARAM_LENGTH		8

#define	MT88E39_DCLK							RD		/* RD of STC */
#define	MT88E39_DATA							P0_3	//WR	/* WR of STC */
#define	MT88E39_DATA_ENABLE					T1		/* enable U58:74HC245  */


#define	PSTN_MAX_GROUP_ID		4

enum
{
	PSTN_RINGER_DISABLE		=	0,	/* PSTN ringer is not work */
	PSTN_RINGER_ON_PENDING	=	1,	/* PSTN ringer is in period of OFF */
	PSTN_RINGER_ON_DETECT	=	2,	/* PSTN ringer is in period of ON */
	PSTN_RINGER_OFF_PENDING	=	3,
	PSTN_RINGER_OFF_DETECT	=	4,
	PSTN_RINGER_BLOCKED		=	5,	/* CAR is processed now */
};

#define	PSTN_SET_BUSY(devIndex)	\
		PBX_CLEAR_FLAGS(pbxHwStatus.trackFreeStatus, GET_PSTN_NUMBER(devIndex))

#define	PSTN_SET_FREE(devIndex)	\
		PBX_SET_FLAGS(pbxHwStatus.trackFreeStatus, GET_PSTN_NUMBER(devIndex))

#define	PSTN_IS_FREE(devIndex)	\
		PBX_CHECK_FLAGS(pbxHwStatus.trackFreeStatus, GET_PSTN_NUMBER(devIndex))
#if 0
#define	PSTN_DISABLE(devIndex) \
		PBX_CLEAR_FLAGS(baseConfigs.trunkStatusFlags, GET_PSTN_NUMBER(devIndex))

#define	PSTN_ENABLE(devIndex) \
		PBX_SET_FLAGS(baseConfigs.trunkStatusFlags, GET_PSTN_NUMBER(devIndex))
#endif
	
#define	PSTN_IS_DISABLE(devIndex) \
		(!PBX_CHECK_FLAGS(baseConfigs.trunkStatusFlags, GET_PSTN_NUMBER(devIndex)) )
	
#define	PSTN_IS_CONSOLE_MODE()\
		(boolTrunkMode == TRUNK_MODE_CONSOLE)

#define	PSTN_IS_DIRECT_MODE()\
		(boolTrunkMode == TRUNK_MODE_DIRECT)

#define	PSTN_GROUP_CALL_IS_ON() \
		(boolGroupCall == TRUE)

#define	PSTN_MY_VIRTUAL_CONSOLE(devIndex)	\
		(baseConfigs.trunkVirtualConsoles[GET_PSTN_NUMBER(devIndex)] )

#define	PSTN_MY_GROUP_ID(devIndex)	\
		(baseConfigs.trunkBindingExtGroup[GET_PSTN_NUMBER(devIndex)] )


/* Direct Inward Dial */
#define	PSTN_IS_DID(devIndex)	\
		(PSTN_GROUP_CALL_IS_ON() ||(PSTN_IS_CONSOLE_MODE()&& deviceStatus[]))

#define	PSTN_RINGER_IS_TIMEOUT(index) \
		(pstnRingerTimeout[index] <= 0)
		
#define	PSTN_RINGER_TIMER_START(index, timeout)	\
		(pstnRingerTimeout[index] = (timeout) )

#define	PSTN_RINGER_NEW_STATE(index, state, timeout)	\
	do{	pstnRingerCounter[index] = 0;	\
		pstnPending[index] = state;	\
		PSTN_RINGER_TIMER_START(index, timeout);	\
		}while(0)

#define	PSTN_CALLID_IS_ON()	\
		(boolCallIdMode == TRUE)

#endif

