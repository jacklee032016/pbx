/*
* $Id: nvram_store.c,v 1.6 2007/07/03 06:13:31 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"
#include "_uart_handlers.h"

void nvram_save_int_value(INT8U cmd, INT8U value)
{
	switch(cmd)
	{
		case CMD_CFG_TRACK_MODE:
			SET_GLOBAL_OPTION(baseConfigs.globalOptions, value, MISC_BIT_TRUNK_MODE);
			break;
		case CMD_CFG_NODIAL_ACTION:
			SET_GLOBAL_OPTION(baseConfigs.globalOptions, value, MISC_BIT_NODIAL);
			break;
		case CMD_CFG_LINE2LINE:
			SET_GLOBAL_OPTION(baseConfigs.globalOptions, value, MISC_BIT_LINE2LINE);
			break;
		case CMD_CFG_DIFFER_RINGING:
			SET_GLOBAL_OPTION(baseConfigs.globalOptions, value, MISC_BIT_DIFFER_RINGING);
			break;
		case CMD_CFG_CALLER_ID_MODE:
			SET_GLOBAL_OPTION(baseConfigs.globalOptions, value, MISC_BIT_CALL_ID);
			break;
		case CMD_CFG_GROUP_CALL:
			SET_GLOBAL_OPTION(baseConfigs.globalOptions, value, MISC_BIT_GROUP_CALL);
			break;
		case CMD_CFG_SPECIAL_FIRST:
			SET_GLOBAL_OPTION(baseConfigs.globalOptions, value, MISC_BIT_SPECIAL_FIRST);
			break;

		case CMD_CFG_PSTN_PREFIX:
			if(value<1 || value >DIAL_DIGIT_SPECIAL )
				value = DIAL_DIGIT_ZERO;
			SET_PSTN_PREFIX(value );

			break;
		case CMD_CFG_IP_PREFIX:
			if(value<1 || value >DIAL_DIGIT_SPECIAL )
				value = DIAL_DIGIT_SPECIAL;
			baseConfigs.ipPrefix.length = 1;
			baseConfigs.ipPrefix.value[0] = (PBX_GET_LOW_NIBBLE(value)<<4);
			NVRAM_MODIFY_GLOBAL();
			
			break;

		case CMD_CFG_SIGNAL_TYPE:
			SET_SIGNAL_TYPE(value);
			break;

		case CMD_CFG_EXT_BUSY_EXT:
			if(IS_EXT_DEVICE(value) )
			{
				extConfigs.busyBranchs[value] = PBX_VALUE_INVALIDATE;
				NVRAM_MODIFY_EXT();
			}	
			break;
		case CMD_CFG_EXT_SECRECT_EXT:
			if( IS_EXT_DEVICE(value) )
			{
				extConfigs.secretBranchs[value] = PBX_VALUE_INVALIDATE;
				NVRAM_MODIFY_EXT();
			}
			break;
			
		default:
			break;
	}

}

void nvram_save_2_int_value(INT8U cmd, INT8U index, INT8U value)
{
	if(value == 0XFE)
		value = PBX_VALUE_INVALIDATE;
	
	switch(cmd)
	{
		case CMD_CFG_ACCOUNT_AUTHOR:
			if(index>=0 && index< PBX_ACCOUNT_NUMBER)
			{
				if(value<EXT_AUTHOR_ALL||value>EXT_AUTHOR_DENY_ALL)
					value = EXT_AUTHOR_ALL;
				baseConfigs.accounts[index].authority = value|(baseConfigs.accounts[index].authority&SET_BIT(1, ACCOUNT_BIT_FREE) );
				NVRAM_MODIFY_GLOBAL();
			}
			break;

		/* PSTN LINE(TRACK) */			
		case CMD_CFG_PSTN_FLAG:
			if(index>=0 && index<PBX_FXO_COUNT)
			{
				SET_GLOBAL_OPTION(baseConfigs.trunkStatusFlags, value, index);	
				NVRAM_MODIFY_GLOBAL();
			}
			break;
		case CMD_CFG_PSTN_GROUP:
			if(index>=0 && index<PBX_FXO_COUNT)
			{
				if(value<1 || value>PSTN_MAX_GROUP_ID)
					value = PBX_VALUE_INVALIDATE;
				baseConfigs.trunkBindingExtGroup[index] = value;
				NVRAM_MODIFY_GLOBAL();
			}
			break;
		case CMD_CFG_PSTN_CONSOLE:
			if(index>=0 && index<PBX_FXO_COUNT)
			{
				if(!IS_EXT_DEVICE(value) )
					value = PBX_VALUE_INVALIDATE;
				baseConfigs.trunkVirtualConsoles[index] = value;	/* EXT for this PSTN line */
				NVRAM_MODIFY_GLOBAL();
			}
			break;

		/************* config items for EXTs ************************/
		case CMD_CFG_EXT_AUTHORITY:
			if(value<EXT_AUTHOR_ALL || value>EXT_AUTHOR_INTERNAL)
				value = EXT_AUTHOR_ALL;
			if(IS_EXT_DEVICE(index) )
			{
				extConfigs.authorities[index] = value;
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_TIMEOUT:
			if(IS_EXT_DEVICE(index) )
			{
				extConfigs.callLengths[index] = value;	/* unit of minutes in IP side, here is in unit of 0.1s  */
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_CALLER_ID:
			if(IS_EXT_DEVICE(index) )
			{
				SET_GLOBAL_OPTION(extConfigs.isCallerIdOnFlags, value, index);
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_BUSY_EXT:
			if(IS_EXT_DEVICE(index) && 
				(IS_EXT_DEVICE(value)||value ==PBX_VALUE_INVALIDATE)&& 
				(index!= value))
			{
				extConfigs.busyBranchs[index] = value;
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_SECRECT_EXT:
			if(IS_EXT_DEVICE(index) && 
				(IS_EXT_DEVICE(value)||value ==PBX_VALUE_INVALIDATE)&& 
				(index!= value))
			{
				extConfigs.secretBranchs[index] = value;
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_DELAY_2_LINE:
			if(IS_EXT_DEVICE( index) )
			{
				extConfigs.delayOutTime[index] = value;
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_DIALOUT_MODE:
			if(IS_EXT_DEVICE(index) )
			{
				if(value == 0 )
					value = DIAL_DIGIT_ZERO;
				else if(value==1 )
					value = EXT_FIRST_PSTN;
				else if(value >0 && value < 9)
					value = EXT_FIRST_IP;
				
				extConfigs.dialOutMethod[index] = value;
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_LEFT_TIME:
			if(IS_EXT_DEVICE(index) )
			{
				extConfigs.leftTranferTimes[index] = value;
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_CALLWAIT:
			if(IS_EXT_DEVICE(index) )
			{
				SET_GLOBAL_OPTION(extConfigs.callWaitFlags, value, index);
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_NODISTURB:
			if(IS_EXT_DEVICE(index) )
			{
				SET_GLOBAL_OPTION(extConfigs.noDisturbFlags, value, index);
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_GROUP:
			if(IS_EXT_DEVICE(index) )
			{
				if(value<1 || value>PSTN_MAX_GROUP_ID)
					value = PBX_VALUE_INVALIDATE;
				extConfigs.groups[index] = value;
				NVRAM_MODIFY_EXT();
			}
			break;
		case CMD_CFG_EXT_DIFFER_RINGING:
			if(IS_EXT_DEVICE(index) )
			{
				SET_GLOBAL_OPTION(extConfigs.isDifferRingFlags, value, index);
				NVRAM_MODIFY_EXT();
			}
			break;

		case CMD_CFG_IP_PREFIX:
			if( (value>0 && value <= DIAL_DIGIT_SPECIAL) && (index>0 && index <= DIAL_DIGIT_SPECIAL) )
				value = DIAL_DIGIT_SPECIAL;
			baseConfigs.ipPrefix.length = 2;
			baseConfigs.ipPrefix.value[0] = (PBX_GET_LOW_NIBBLE(index)<<4)|PBX_GET_LOW_NIBBLE(value);
			NVRAM_MODIFY_GLOBAL();
			break;

		default:
			break;
	}
}


void nvram_save_values(INT8U cmd, INT8U xdata *value, INT8U length)
{
	switch(cmd)
	{
		/********* for coded evert digits in one byte *****************/
		case CMD_CFG_SYS_PASSWD:
			if( (length >= 0) && (length <= 2*PHONE_NUMBER_BUF_SIZE) )
			{
				baseConfigs.passwordSystem.length = length;
				ipdigit_2_pbxdigit(baseConfigs.passwordSystem.value, value, (IS_ODD(length))?(length/2+1):length/2);
				NVRAM_MODIFY_GLOBAL();
			}
			break;
		case CMD_CFG_LINE2LINE_PASSWD:
			if( (length >=0 ) && (length <= 2*PBX_ACCOUNT_NUMBER) )
			{
				baseConfigs.passwordLine2Line.length = length;
				ipdigit_2_pbxdigit( baseConfigs.passwordLine2Line.value, value, (IS_ODD(length))?(length/2+1):length/2);
				NVRAM_MODIFY_GLOBAL();
			}
			break;
			
		case CMD_CFG_IP_PREFIX:
			if( (length>0) && (length <= 2*PHONE_NUMBER_BUF_SIZE) )
			{
				ipdigit_2_pbxdigit( baseConfigs.ipPrefix.value, value, length);
				baseConfigs.ipPrefix.length = length;
				NVRAM_MODIFY_GLOBAL();
			}
			break;

		case CMD_CFG_ACCOUNT_PASSWD:
			if( value[0]>=0 && value[0]< PBX_ACCOUNT_NUMBER && 
				(length-1)>0 && (length-1) <= 2*PHONE_NUMBER_BUF_SIZE )
			{
				ipdigit_2_pbxdigit( baseConfigs.accounts[value[0]].passwd.value, value+1, length-1);
				baseConfigs.accounts[value[0]].passwd.length = length -1 ;
				NVRAM_MODIFY_GLOBAL();
			}
			break;
			
		/* should be enhanced by check phone number is validate, such as duplicated phone number */
		case CMD_CFG_EXT_PHONENUMER:
			if( IS_EXT_DEVICE(value[0]) && 
				(length-1)>0 && (length-1) <= 2*PHONE_NUMBER_BUF_SIZE )
			{
				ipdigit_2_pbxdigit( extConfigs.phoneNumbers[value[0]].value, value+1, length-1);
				extConfigs.phoneNumbers[value[0]].length = length -1 ;
				NVRAM_MODIFY_EXT();
			}
			break;

		case CMD_CFG_SPECIAL_SERVICE:
			if(value[0]>=0 && value[0]< PBX_SPECIAL_SERVICE_COUNT&& 
				(length-1)>0 && (length-1) <= 2*PHONE_NUMBER_BUF_SIZE )
			{
				ipdigit_2_pbxdigit( baseConfigs.specialServices[value[0]].value, value+1, length-1);
				baseConfigs.specialServices[value[0]].length = length -1 ;
				NVRAM_MODIFY_GLOBAL();
			}
			break;

		case CMD_CFG_EXT_TRANSFER_DENY:
			if(value[0]>=0 && value[0]< STC_BLACKLIST_COUNT&& 
				(length-1)>0 && (length-1) <= 2*PHONE_DIGIT_BUFFER_SIZE )
			{
//				ipdigit_2_pbxdigit( baseConfigs.blackList[value[0]].value, value+1, length-1);
				pbx_x_str_copy(baseConfigs.blackList[value[0]].value, value+1, length -1 );
				baseConfigs.blackList[value[0]].length = length -1 ;
				NVRAM_MODIFY_GLOBAL();
			}
			break;

		/********* for compressed BCD coded digits *****************/
		case CMD_CFG_EXT_TRANSFER_LEFT:
			if( IS_EXT_DEVICE(value[0]) && 
				(length-2)>0 && (length-2) <= 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				extTransfers.leftTransfers[value[0]].type = value[1];
				extTransfers.leftTransfers[value[0]].digits.length = length -2;			
				pbx_x_str_copy(extTransfers.leftTransfers[value[0]].digits.value, value+2, length -2 );
				NVRAM_MODIFY_LEFT();
			}
			break;

		case CMD_CFG_EXT_TRANSFER_NIGHT:
			if( IS_EXT_DEVICE(value[0])  && 
				(length-2)>0 && (length-2) <= 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				extTransfers.nightTransfers[value[0]].type = value[1];
				extTransfers.nightTransfers[value[0]].digits.length = length -2;
				pbx_x_str_copy(extTransfers.nightTransfers[value[0]].digits.value, value+2, length -2 );
				NVRAM_MODIFY_NIGHT();
			}
			break;

		case CMD_CFG_EXT_TRANSFER_UNCONDITIONAL:
			if(IS_EXT_DEVICE(value[0])  && 
				(length-2)>0 && (length-2) <= 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				extTransfers.unconditionalTransfers[value[0]].type = value[1];
				extTransfers.unconditionalTransfers[value[0]].digits.length = length -2;
				pbx_x_str_copy(extTransfers.unconditionalTransfers[ value[0]].digits.value, value+2, length -2 );
				NVRAM_MODIFY_UNCONDITIONAL();
			}
			break;

		case CMD_CFG_NIGHT_SERVICE_TIME:
			if(length != sizeof(night_service_time_t) )
				break;
			pbx_x_str_copy((INT8U *)&baseConfigs.nightServiceTime, value, length);
			NVRAM_MODIFY_GLOBAL();
			break;


		default:
			break;
	}
}

void nvram_save_values_4_program(INT8U cmd, INT8U xdata *value, INT8U length)
{
	INT8U data index, transferType;
	
	index = PBX_GET_HIGH_NIBBLE(value[0]) -1;/* from 1~max to 0~max-1*/
	transferType = PBX_GET_LOW_NIBBLE(value[0]);
	if(transferType == DIAL_DIGIT_ZERO)
	{
		transferType = EXT_TRANSFER_TYPE_UNKNOWN;
	}
	else if(transferType == 1)
	{
		transferType = EXT_TRANSFER_TYPE_IP;
	}
	else
	{
		transferType = EXT_TRANSFER_TYPE_PSTN;
	}
	
	switch(cmd)
	{
		/********* for coded evert digits in one byte *****************/
		case CMD_CFG_SYS_PASSWD:
			if( (length>= 0) && (length <= 2*PHONE_NUMBER_BUF_SIZE) )
			{
				baseConfigs.passwordSystem.length = length;
				pbx_x_str_copy(baseConfigs.passwordSystem.value, value, (IS_ODD(length))?(length/2+1):length/2);
				NVRAM_MODIFY_GLOBAL();
			}
			break;
		case CMD_CFG_LINE2LINE_PASSWD:
			if( (length >= 0) && (length <= 2*PHONE_NUMBER_BUF_SIZE) )
			{
				baseConfigs.passwordLine2Line.length = length;
				pbx_x_str_copy( baseConfigs.passwordLine2Line.value, value, (IS_ODD(length))?(length/2+1):length/2 );
				NVRAM_MODIFY_GLOBAL();
			}
			break;
			

		case CMD_CFG_ACCOUNT_PASSWD:
			if( index >=0 && index< PBX_ACCOUNT_NUMBER && 
				length>= 0 && length <= 2*PHONE_NUMBER_BUF_SIZE )
			{
				pbxdigit_odd_copy( baseConfigs.accounts[index].passwd.value, value, length );
				baseConfigs.accounts[index].passwd.length = length;
				NVRAM_MODIFY_GLOBAL();
			}
			break;
			
		/* should be enhanced by check phone number is validate, such as duplicated phone number */
		case CMD_CFG_EXT_PHONENUMER:
			if( IS_EXT_DEVICE(index) && length == 3)
/*				length>0 && length <= 2*PHONE_NUMBER_BUF_SIZE )*/
			{
				pbxdigit_odd_copy( extConfigs.phoneNumbers[index].value, value, length);
				extConfigs.phoneNumbers[index].length = length;
				NVRAM_MODIFY_EXT();
			}
			break;

		case CMD_CFG_SPECIAL_SERVICE:
			if(index>=0 && index< PBX_SPECIAL_SERVICE_COUNT&& 
				length>= 0 && length <= 2*PHONE_NUMBER_BUF_SIZE )
			{
				pbxdigit_odd_copy( baseConfigs.specialServices[index].value, value, length);
				baseConfigs.specialServices[index].length = length;
				NVRAM_MODIFY_GLOBAL();
			}
			break;

		case CMD_CFG_EXT_TRANSFER_DENY:
			if(index>=0 && index< STC_BLACKLIST_COUNT&& 
				length >= 0 && length <= 2*PHONE_DIGIT_BUFFER_SIZE )
			{
//				ipdigit_2_pbxdigit( baseConfigs.blackList[value[0]].value, value+1, length-1);
				pbxdigit_odd_copy(baseConfigs.blackList[index].value, value, length );
				if(IS_ODD(length) )
					baseConfigs.blackList[index].value[length/2] |= 0X0F;
				baseConfigs.blackList[index].length = length;
				NVRAM_MODIFY_GLOBAL();
			}
			break;

		/********* for compressed BCD coded digits *****************/
		case CMD_CFG_EXT_TRANSFER_LEFT:
			length = length -1;
			if( IS_EXT_DEVICE(index) && 
				length >= 0 && length <= 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				extTransfers.leftTransfers[index].type = transferType;
				extTransfers.leftTransfers[index].digits.length = length;			
				pbx_x_str_copy(extTransfers.leftTransfers[index].digits.value, value+1, (IS_ODD(length))? (length/2+1): (length/2) );
				NVRAM_MODIFY_LEFT();
			}
			break;

		case CMD_CFG_EXT_TRANSFER_NIGHT:
			length = length -1;
			if( IS_EXT_DEVICE(index)  && 
				length>= 0 && length<= 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				extTransfers.nightTransfers[index].type = transferType;
				extTransfers.nightTransfers[index].digits.length = length;
				pbx_x_str_copy(extTransfers.nightTransfers[index].digits.value, value+1,(IS_ODD(length))? (length/2+1): (length/2) );
				NVRAM_MODIFY_NIGHT();
			}
			break;

		case CMD_CFG_EXT_TRANSFER_UNCONDITIONAL:
			length = length -1;/*except 'type' field */
			if(IS_EXT_DEVICE(index)  && 
				length >= 0 && length <= 2*PHONE_DIGIT_BUFFER_SIZE )
			{
				extTransfers.unconditionalTransfers[index].type = transferType;
				extTransfers.unconditionalTransfers[index].digits.length = length;
				pbx_x_str_copy(extTransfers.unconditionalTransfers[index].digits.value,value+1, (IS_ODD(length))? (length/2+1): (length/2) );
				NVRAM_MODIFY_UNCONDITIONAL();
			}
			break;

		default:
			break;
	}
}

