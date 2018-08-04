/*
* $Id: timerTasks.c,v 1.4 2007/08/11 19:26:27 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

/* in unit of 100ms */
void	_account_timeout(void)
{
	INT8U data i;
	
	for(i=0;i<2;i++)
	{
		if(--zhanghao_jishi[i]==0)
		{
			if(((baseConfigs.accounts[i].authority&ACCOUNT_FREE_BIT)==0)
				&&(baseConfigs.accounts[i].authority&ACCOUNT_ONHOOK_BIT)/*0x08:has on-hook */)
			{
				baseConfigs.accounts[i].authority|=ACCOUNT_FREE_BIT;/*0x80 delay from onhook about 20s, then set highest bit */
			}
		}
	}
}


void _callTimerTask(void)
{
	INT8U data	i;
	
	for(i=0; i<sizeof(callDevices)/sizeof(pbx_call_device_t); i++)
	{
		if(callDevices[i].timeout > 0)
		{
			callDevices[i].timeout--;
			if(callDevices[i].timeout <= 0)
			{
				CALL_EVENT_EMIT(i, PBX_EVENT_TIMEOUT);
			}
		}
	}
}

/* all timer tasks */
void	pbxTimerTasks(void)
{
	INT8U data i;

	if(bool25msFlag)
	{
		bool25msFlag = FALSE;
		hwReadDtmfDecoders();
	}
		
	if(bool50msFlag)
	{
		bool50msFlag = FALSE;
		
		if(pbxHwStatus.buzzerTimeout > 0)
		{
			pbxHwStatus.buzzerTimeout--;
			if(pbxHwStatus.buzzerTimeout == 0)
			{
				pbx_buzzer_ctrl(FALSE);
			}
		}
		
		extTimerTask50ms();
		pstnTimerTask50ms();
//		pbx_statemachine_handle();

		if(uart.ipGetConfigIndex!= PBX_VALUE_INVALIDATE)/* IP want to retrieve all PBX configuration data */
			net_service_config();
	}
	
	if(bool100msFlag)
	{
		bool100msFlag = FALSE;
		_account_timeout();

		_callTimerTask();
//		pbx_statemachine_handle();
		
		for(i=0; i<ENCODE_CHANNEL_NUMBER; i++)
		{
			if( (encodingChannels[i].timeout) > 0 && (encodingChannels[i].owner != PBX_VALUE_INVALIDATE) )
			{
				encodingChannels[i].timeout--;
				if(encodingChannels[i].timeout == 0)
				{
					dtmfEncoderPlayStep(i);
				}
			}
		}
		callerIdSendFlags=!callerIdSendFlags;

	}
	
	if(bool500msFlag)
	{
		bool500msFlag = FALSE;
//		WATCHDOG_FEED();
	}
	
	if(bool1MinuteFlag)
	{
		bool1MinuteFlag = FALSE;
//		pbx_reset_systime();
		pbx_check_night_service_time();

//		if( ipGetConfigIndex == PBX_VALUE_INVALIDATE)
			saveNvram();
	}
}	

