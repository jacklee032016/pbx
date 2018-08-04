/*
* $Id: pbx_cfg.h,v 1.2 2007/07/01 21:22:12 lizhijie Exp $
*/
#ifndef	__PBX_CFG_H__
#define	__PBX_CFG_H__

#define	NVRAM_BIT_ID								0
#define	NVRAM_BIT_GLOBAL							1
#define	NVRAM_BIT_EXT								2
#define	NVRAM_BIT_TRANSFER_NIGHT				3
#define	NVRAM_BIT_TRANSFER_UNCONDITIONAL		4
#define	NVRAM_BIT_TRANSFER_LEFT					5

/* tag it as modified */
#define	NVRAM_MODIFY_GLOBAL()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_GLOBAL)

#define	NVRAM_MODIFY_EXT()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_EXT)

#define	NVRAM_MODIFY_NIGHT()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_NIGHT)

#define	NVRAM_MODIFY_UNCONDITIONAL()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_UNCONDITIONAL)

#define	NVRAM_MODIFY_LEFT()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_LEFT)

/* check */
#define	NVRAM_CHECK_GLOBAL()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_GLOBAL)
	
#define	NVRAM_CHECK_EXT()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_EXT)
	
#define	NVRAM_CHECK_NIGHT()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_NIGHT)
	
#define	NVRAM_CHECK_UNCONDITIONAL()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_UNCONDITIONAL)
	
#define	NVRAM_CHECK_LEFT()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_LEFT)

/* clear */
#define	NVRAM_CLEAR_GLOBAL()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_GLOBAL)
	
#define	NVRAM_CLEAR_EXT()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_EXT)
	
#define	NVRAM_CLEAR_NIGHT()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_NIGHT)
	
#define	NVRAM_CLEAR_UNCONDITIONAL()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_UNCONDITIONAL)
	
#define	NVRAM_CLEAR_LEFT()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_LEFT)
	

#define	SET_GLOBAL_OPTION(flags, value, bit)	\
	do{	if ((value)==0 || (value)== PBX_VALUE_INVALIDATE ){	\
		PBX_CLEAR_FLAGS((flags), (bit));}	\
		else {	PBX_SET_FLAGS((flags), (bit)); }\
			NVRAM_MODIFY_GLOBAL();	\
		}while(0)

#define	SET_IP_PREFIX(digitValue)	\
	do{	baseConfigs.ipPrefix.length = 1;	\
		baseConfigs.ipPrefix.value[0] = digitValue;\
		NVRAM_MODIFY_GLOBAL();	\
		}while(0)

#define	SET_PSTN_PREFIX(digitValue)	\
	do{	baseConfigs.pstnPrefix = digitValue;\
		NVRAM_MODIFY_GLOBAL();	\
		}while(0)

#define	SET_SIGNAL_TYPE(digitValue)	\
	do{	if( (digitValue != SIGNAL_TYPE_CHINESE) && (digitValue != SIGNAL_TYPE_JAPANESE) )\
		{		digitValue = SIGNAL_TYPE_JAPANESE;}\
			baseConfigs.signalType = digitValue;	\
		NVRAM_MODIFY_GLOBAL();	\
		}while(0)


#define	SET_LINE_STATUS(index, value)	\
	do{	if(index>=0 && index<PBX_FXO_COUNT)	{	\
		SET_GLOBAL_OPTION(baseConfigs.trunkStatusFlags, value, index);	}\
		}while(0)
	
#endif

