/*
* $Id: cfgProgramCmds.c,v 1.3 2007/08/10 20:14:38 lizhijie Exp $
*/

#pragma	ot(9,speed)
//#pragma large
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

/* All commands of config PBX are defined in this file */

void programSucess(INT8U extIndex)
{
	INT8U	data channel;

	CONNECT(extIndex, CHANNEL_COL_MUSIC);
//	CALL_TIMER_START(extIndex, 10*PBX_100MS);
	PBX_PROGRAM_MODE_CANCEL();
	
	if( (channel = dtmfDecodingChannelFind(extIndex)) != PBX_VALUE_INVALIDATE)
	{
		dtmfDecodingChannelFree(extIndex);
	}
}

/* length of dialing digits which not contains dialing pattern but contained index value */
INT8U __findEndChar(phone_digits_t xdata *dialingDigits)
{
#if 0	
	INT8U data i;

	for(i=0; i< (dialingDigits->length-4)/2; i++)
	{
		/* only even digits is checked here, odd digits is checked by following */
		if(PBX_GET_LOW_NIBBLE(dialingDigits->value[i+2])== DIAL_DIGIT_SPECIAL)
			return (i+1)*2-1;
	}
#endif

	if( IS_ODD(dialingDigits->length) )
	{
		if(PBX_GET_HIGH_NIBBLE(dialingDigits->value[dialingDigits->length/2])== DIAL_DIGIT_SPECIAL)
		{/* not contains length of '#' */
			return (dialingDigits->length-4-1 );
		}
	}
	else
	{
		if(PBX_GET_LOW_NIBBLE(dialingDigits->value[dialingDigits->length/2-1])== DIAL_DIGIT_SPECIAL)
			return (dialingDigits->length-4-1 );
	}

	return PBX_VALUE_INVALIDATE;
}


/* one parameter and the diaied digits are '1*' */
INT8U	program_cfg_global_options(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits)
{
	INT8U	data value;

	if(dialingDigits->length >= 5)
	{
		value = PBX_GET_HIGH_NIBBLE(dialingDigits->value[2]);
		if((value >= DIAL_DIGIT_ZERO) && (cmd != CMD_CFG_IP_PREFIX) && (cmd != CMD_CFG_PSTN_PREFIX) )
			value = 0;
		
		globalConfig = baseConfigs.globalOptions;
		nvram_save_int_value(cmd, value);
		programSucess(extIndex);

//		CALL_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	}
	return PBX_STATE_CONTINUE;
}

INT8U	program_cfg_2_params(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits)
{
	INT8U	data index, value;
	
	index = PBX_GET_HIGH_NIBBLE(dialingDigits->value[2]) -1;
	if(dialingDigits->length >= 6)
	{
		if(cmd == PROGRAM_CMD_OP_REPLACE )
		{
			/* EXT checking */
			index = pbx_find_ext_by_number( &dialingDigits->value[2], dialingDigits->length -4 );
			if(IS_EXT_DEVICE(index) && callDevices[index].status == PBX_STATE_RINGING )
			{
				CALL_EVENT_EMIT(index, PBX_SIGNAL_CANCEL);
				
				CALL_MY_PEER(extIndex) = CALL_MY_PEER(index);
				CALL_MY_PEER(CALL_MY_PEER(extIndex) ) = extIndex;
				CALL_MY_PEER(index) = PBX_VALUE_INVALIDATE;

				if( voiceChannelConnect(extIndex, CALL_MY_PEER(extIndex) ))
				{
					CALL_PEER_EVENT(extIndex, PBX_SIGNAL_OK);

					PBX_PROGRAM_MODE_CANCEL();
					return PBX_STATE_IN_CALL;
				}

			}
			else
			{
				goto exitProgram;
			}
		}
		
//		else
		{
			value = PBX_GET_LOW_NIBBLE(dialingDigits->value[2]);
			if(value >= DIAL_DIGIT_ZERO)
				value = 0;
			
			if(cmd == CMD_CFG_PSTN_CONSOLE || cmd == CMD_CFG_EXT_BUSY_EXT ||
				cmd == CMD_CFG_EXT_SECRECT_EXT ) 
			{
				if(value != 0)
				{
					if(!IS_EXT_DEVICE(value-1) )
						goto exitProgram;
					else if(value != 0)
					{
						value = value -1;
					}
					
					if(cmd == CMD_CFG_EXT_SECRECT_EXT || cmd == CMD_CFG_EXT_BUSY_EXT)
					{
						if(value == index)
							goto exitProgram;
					}
				}
				else
					value = PBX_VALUE_INVALIDATE;

			}
			
			nvram_save_2_int_value(cmd, index, value);
			programSucess(extIndex);

			return PBX_STATE_CONTINUE;
		}
	}
	else if(dialingDigits->length == 5)
	{
		if(cmd== CMD_CFG_PSTN_FLAG || cmd == CMD_CFG_PSTN_GROUP || cmd == CMD_CFG_PSTN_CONSOLE)
		{
			if(index < 0 || index>= PBX_FXO_COUNT)
				goto exitProgram;
		}
		else if(cmd == CMD_CFG_ACCOUNT_AUTHOR)
		{
			if(index<0 ||index>= PBX_ACCOUNT_NUMBER)
				goto exitProgram;
		}
		else if(cmd == PROGRAM_CMD_OP_REPLACE)
		{
			if(PBX_GET_HIGH_NIBBLE(dialingDigits->value[2]) != DIAL_DIGIT_ASTREIC )
				goto exitProgram;
		}
		else 
		{
			if(!IS_EXT_DEVICE(index) )
				goto exitProgram;
		}
	}

	return PBX_STATE_CONTINUE;

exitProgram:		
	CALL_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	PBX_PROGRAM_MODE_CANCEL();
	return PBX_STATE_CONTINUE;

}


INT8U	program_cfg_passwd(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits)
{
	INT8U	data length;
	
	if(dialingDigits->length >= 5)
	{
		length = __findEndChar(dialingDigits);
		if(length  == PBX_VALUE_INVALIDATE && dialingDigits->length >= (4+4) )
		{
			length = 4;
		}

		if(length != PBX_VALUE_INVALIDATE)
		{
			if(IS_ODD(length) )
				dialingDigits->value[2+length/2] &= 0XF0;
			nvram_save_values_4_program(cmd, &(dialingDigits->value[2]), length );
			programSucess(extIndex);
		}
	}
	
	return PBX_STATE_CONTINUE;
}

INT8U	program_cfg_index_value(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits)
{
	INT8U	data index, length, type;
	
	if(dialingDigits->length >= 5)
	{
		index = PBX_GET_HIGH_NIBBLE(dialingDigits->value[2]);
		type = PBX_GET_LOW_NIBBLE(dialingDigits->value[2]);
		length = __findEndChar(dialingDigits);
		if(length != PBX_VALUE_INVALIDATE)
			length = length -1;/* except length of index */
		
		switch (cmd)
		{
			case CMD_CFG_ACCOUNT_PASSWD:
				if(index<1 || index > PBX_ACCOUNT_NUMBER )
					goto exitProgram;
				if(length == PBX_VALUE_INVALIDATE && dialingDigits->length >= (4+4+1) )
				{
					length = 4;
				}
				break;
				
			case CMD_CFG_SPECIAL_SERVICE:
				if(index<1 || index > PBX_SPECIAL_SERVICE_COUNT)
					goto exitProgram;
				if(length  == PBX_VALUE_INVALIDATE && dialingDigits->length >= (4+2*PHONE_NUMBER_BUF_SIZE+1) )
				{
					length = 4;
				}
				break;
			case CMD_CFG_EXT_PHONENUMER:
				if(index< 1 || index > PBX_FXS_COUNT)
					goto exitProgram;
				if( (length  == PBX_VALUE_INVALIDATE ) && 
					(dialingDigits->length >= (4+2*PHONE_NUMBER_BUF_SIZE+1 -1)) )
				{/* only 3 digits for EXT phone number */
					length = 3;/*4;*/
				}
				
				if(length>0 && length < 3)
				{
					goto exitProgram;
				}
				
				break;
			case CMD_CFG_EXT_TRANSFER_DENY:
				if(index<1 || index > STC_BLACKLIST_COUNT)
					goto exitProgram;
#if 0				
				if(length != PBX_VALUE_INVALIDATE)
				{
					if( IS_ODD(length) )/* clear last nibble */
						dialingDigits->value[dialingDigits->length/2-1] |= 0X0F;
				}
				else
#endif					
					if(length  == PBX_VALUE_INVALIDATE && dialingDigits->length >= (4+2*PHONE_DIGIT_BUFFER_SIZE+1) )
				{
					length = 18;
				}
				break;
			default:/* night service transfer, unconditional and left transfer */
				if(index<1 || index > PBX_FXS_COUNT)
					goto exitProgram;
				if(type == DIAL_DIGIT_ZERO)
				{
					length = 1;/* length for 'type' field */
				}
				else if(length != PBX_VALUE_INVALIDATE)
				{
					if(length == 1)
					{/* for Speed_dial of IP transfer */
						length = PBX_VALUE_INVALIDATE;
						if(dialingDigits->length > 7)
						{
							if( !IS_ODD(dialingDigits->length) )/* clear last nibble */
							{
								dialingDigits->value[dialingDigits->length/2-1] |= 0X0F;
								if(PBX_GET_HIGH_NIBBLE(dialingDigits->value[dialingDigits->length/2-1]) == DIAL_DIGIT_SPECIAL )
									length = dialingDigits->length - 5;
							}
							else
							{
								if(PBX_GET_LOW_NIBBLE(dialingDigits->value[dialingDigits->length/2-1]) == DIAL_DIGIT_SPECIAL )
									length = dialingDigits->length - 5;
							}

							if(length == PBX_VALUE_INVALIDATE && dialingDigits->length >= (4+2*PHONE_DIGIT_BUFFER_SIZE+1+1) )
							{
								length = 18;
							}
						}

					}
					else if( !IS_ODD(dialingDigits->length) )/* clear last nibble */
						dialingDigits->value[dialingDigits->length/2-1] |= 0X0F;

				}
				else if(length == PBX_VALUE_INVALIDATE && dialingDigits->length >= (4+2*PHONE_DIGIT_BUFFER_SIZE+1+1) )
				{
					length = 18;
				}
				
				break;
				
		}

		if( length != PBX_VALUE_INVALIDATE)
		{
			nvram_save_values_4_program(cmd, &(dialingDigits->value[2]), length );
			programSucess(extIndex);
		}
	}

	return PBX_STATE_CONTINUE;
	
exitProgram:		
	CALL_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	PBX_PROGRAM_MODE_CANCEL();
	return PBX_STATE_CONTINUE;
}



INT8U	program_cfg_index_time_value(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits)
{
	INT8U	data index, length, value = PBX_VALUE_INVALIDATE;
	
	if(dialingDigits->length >= 5)
	{
		index = PBX_GET_HIGH_NIBBLE(dialingDigits->value[2]) -1;
		if(!IS_EXT_DEVICE(index))
			goto exitProgram;
		
		if(dialingDigits->length >= 6)
		{

			length = __findEndChar(dialingDigits);
			if(length != PBX_VALUE_INVALIDATE)
				length = length - 1;/* except length of index */
			else if(length == PBX_VALUE_INVALIDATE && dialingDigits->length >= (4+2 +1) )
				length =2 ;

			if(length < 1 )
				goto exitProgram;
			
			if(length == 2)
			{
				value = PBX_GET_LOW_NIBBLE(dialingDigits->value[2]);
				if(value>= DIAL_DIGIT_ZERO)
					value = 0;
				length = PBX_GET_HIGH_NIBBLE(dialingDigits->value[3]);
				if(length >= DIAL_DIGIT_ZERO)
					length = 0;
				value =  value*10 + length;
			}
			else if(length == 1)
			{
				value = PBX_GET_LOW_NIBBLE(dialingDigits->value[2] );
				if(value >= DIAL_DIGIT_ZERO)
					value = 0;
			}

			if(value!= PBX_VALUE_INVALIDATE)
			{
				nvram_save_2_int_value(cmd, index, value);
				programSucess(extIndex);
			}
		}
	}

	return PBX_STATE_CONTINUE;
	
exitProgram:
	CALL_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	PBX_PROGRAM_MODE_CANCEL();
	return PBX_STATE_CONTINUE;
}

INT8U	program_cfg_night_service_time(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits)
{
	INT8U	data length, high;

	cmd = cmd;
	
	if(dialingDigits->length >= 5)
	{
		length = __findEndChar(dialingDigits);

		if(length  == PBX_VALUE_INVALIDATE && dialingDigits->length >= (4+8) )
			length = 8;

		if(length < 8)
		{
			CALL_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
			PBX_PROGRAM_MODE_CANCEL();
		}
		else if(length == 8)
		{
			high = PBX_GET_HIGH_NIBBLE(dialingDigits->value[2]);
			length = PBX_GET_LOW_NIBBLE(dialingDigits->value[2]);
			if(high>= DIAL_DIGIT_ZERO)
				high = 0;
			if(length>= DIAL_DIGIT_ZERO)
				length = 0;
			baseConfigs.nightServiceTime.beginHour = high *10 + length;
			
			high = PBX_GET_HIGH_NIBBLE(dialingDigits->value[3]);
			length = PBX_GET_LOW_NIBBLE(dialingDigits->value[3]);
			if(high>= DIAL_DIGIT_ZERO)
				high = 0;
			if(length>= DIAL_DIGIT_ZERO)
				length = 0;
			baseConfigs.nightServiceTime.beginMiniute = high *10 + length;

			high = PBX_GET_HIGH_NIBBLE(dialingDigits->value[4]);
			length = PBX_GET_LOW_NIBBLE(dialingDigits->value[4]);
			if(high>= DIAL_DIGIT_ZERO)
				high = 0;
			if(length>= DIAL_DIGIT_ZERO)
				length = 0;
			baseConfigs.nightServiceTime.endHour = high *10 + length;

			high = PBX_GET_HIGH_NIBBLE(dialingDigits->value[5]);
			length = PBX_GET_LOW_NIBBLE(dialingDigits->value[5]);
			if(high>= DIAL_DIGIT_ZERO)
				high = 0;
			if(length>= DIAL_DIGIT_ZERO)
				length = 0;
			baseConfigs.nightServiceTime.endMinutes = high *10 + length;

			NVRAM_MODIFY_GLOBAL();
			programSucess(extIndex);

		}
	}

	return PBX_STATE_CONTINUE;
}


