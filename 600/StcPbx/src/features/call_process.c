/*
* $Id: call_process.c,v 1.3 2007/06/27 07:49:03 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>

#include "function_declarations.h"

INT8U	call_dialing_check_dest(INT8U srcDev, phone_digits_t xdata *dialingDigits)
{
	INT8U	data n, dest = PHONE_ERROR;
	
	if(IS_EXT_DEVICE(deviceStatus[srcDev].peerId) ||IS_PSTN_LINE_DEVICE(deviceStatus[srcDev].peerId))
	{/*IP --> EXT and IP-->PSTN */
		return deviceStatus[srcDev].peerId;
	}
	
	/* redialed Digits after IP call has been found */
	if( IS_IP_DEVICE(deviceStatus[srcDev].peerId) )
	{
		if( dialingDigits->length > 0 )
		{
			ip_call_tx_callee_digits_signal(srcDev, dialingDigits );
	//		decodingChannels[channel].digits.length = 0;
		}

		return PHONE_INCOMPLATE;
	}

	/* EXT checking */
	dest = pbx_find_ext_by_number( dialingDigits->value, dialingDigits->length );
	
	if(dest!= PHONE_ERROR)
		return dest;

	/* IP Process */
//		 if( pbx_compare_digits(baseConfigs.ipPrefix.value, decodingChannels[channel].digits.value, baseConfigs.ipPrefix.length) == PHONE_OK)
	if((PBX_GET_HIGH_NIBBLE(dialingDigits->value[0])== DIAL_DIGIT_SPECIAL) ||
		(extConfigs.dialOutMethod[srcDev]==EXT_FIRST_IP))	
	{
		return PHONE_IP;
	}

	/* PSTN Process */
	n = PBX_GET_HIGH_NIBBLE(dialingDigits->value[0]);
	if( ( n==baseConfigs.pstnPrefix ) ||
		(IS_EXT_DEVICE(srcDev) && (n== extConfigs.dialOutMethod[srcDev]) ) )
		//&& (phoneDialoutModes[devIndex]>10) && (is_had_zhanghao(j)!=0xff) ) )
	{
		return PHONE_PSTN;
	}

	return PHONE_ERROR;
}

ext_transfer_digits xdata *call_transfer_check(INT8U extIndex)
{
	if( (extTransfers.unconditionalTransfers[extIndex].type !=EXT_TRANSFER_TYPE_UNKNOWN)
		&& (extTransfers.unconditionalTransfers[extIndex].digits.length> 0) )
	{
		return extTransfers.unconditionalTransfers + extIndex;
	}

	if(0)//nowIsNightServiceTime)
	{
		if( (extTransfers.nightTransfers[extIndex].type !=EXT_TRANSFER_TYPE_UNKNOWN)
			&& (extTransfers.nightTransfers[extIndex].digits.length> 0) )
		{
			return  extTransfers.nightTransfers + extIndex;
		}
	}

	return NULL;		
}


