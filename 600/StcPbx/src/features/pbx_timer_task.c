/*
* $Id: pbx_timer_task.c,v 1.17 2007/07/25 17:50:45 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

INT8U xdata	busyNum[4] = {0,0,0,0};

#if 0
void _busy_tone_detect(void)
{
	INT8U	data i,bytecnt;
	INT8U	data *data cutbuf0;
	BOOL 	lastLevel, currentLevel;	
	
	cutbuf0 = cutbuf;

	for(i=0; i<PBX_FXO_COUNT; i++)
	{
		currentLevel = (BOOL) ((backsoundbuf >> i) & 0x01);/*按位读检测到的电平*/
		bytecnt = *cutbuf0;				/*读取之前检测到的信息*/
		lastLevel = (BOOL) ((bytecnt >> 4) & 0x01);/*读取上次检测到的电平 bytecnt第4位上次电平*/

		if(currentLevel == lastLevel )
		{/* with same level in this 50ms */
			bytecnt++;/* bytecnt 低4位*50ms = 电平持续时间 */
			if((bytecnt&0x0f)>10)/* 相同位信息检测到的次数上限*/
			{/* no level change for about 500ms, no busy tone  */
				busyNum[i]=0;
				bytecnt &= 0x10;	
				bytecnt |= 0x01;	//重新计数
//				bytecnt |= currentLevel;
			}
		}
		else
		{
			bytecnt ^= 0x10;		/* XOR */
			if((bytecnt & 0x0f) < 6)/* 相同位信息检测到的次数下限*/
			{/* level change within 300ms, so no busy tone */
				busyNum[i]=0;
				bytecnt &= 0x10;
				bytecnt |= 0x01;
			}
			else
			{
				//bytecnt += 0x20;	//bytecnt高3位（波形 高+低总次数）+1
				busyNum[i]++;		//忙音计数 repaired by cz 8.16
				bytecnt &= 0xf0;
				bytecnt |= 0x01;
			}
		}
		
//		if((bytecnt >> 5) > 6)//电平有效次数
		if(busyNum[i] > 1)				/* 19 : 忙音计数 repaired by cz 8.16 */
		{
			busyNum[i]=0;			//忙音计数 repaired by cz 8.16
			bytecnt = 0x00;
			/* notify other part */
			cutresult |= (0x01 << i);
		}
		
		*cutbuf0 = bytecnt;

#if 0		
		if (!currentLevel )
		{
//			if (status[i+8][0]==31 || status[i+8][0]==32||status[i+8][0]==37|| status[i+8][0]==38)
			{
				TrackVoice_jishi[i]=(unsigned int )5*60*20; /* 5 minutes */
			}
		}
		else
		{
//			if (TrackVoice_jishi[i] && (status[i+8][0]==31 || status[i+8][0]==32||status[i+8][0]==37|| status[i+8][0]==38) )
			{
//				if (--TrackVoice_jishi[i]==0)
					cutresult |= (0x01 << i);
			
			}
//			else
			{
				TrackVoice_jishi[i]=0;
			}
		}
#endif

		cutbuf0++;
	}
}
#else

#if 0
void _busy_tone_detect(void)
{
	INT8U	data i,bytecnt;
	INT8U	data *data cutbuf0;
	INT8U 	data 	lastLevel, currentLevel;	
	
	cutbuf0 = cutbuf;

	for(i=0; i<1/*PBX_FXO_COUNT*/; i++)
	{
		if((PSTN_IS_FREE((i+PBX_FXS_COUNT)) ) ||(PSTN_IS_DISABLE((i+PBX_FXS_COUNT)) ) )
			continue;
		
		currentLevel =  ((backsoundbuf >> (i) ) & 0x01);
		bytecnt = *cutbuf0;
		lastLevel =  ((bytecnt >> 4) & 0x01);

#if 0		
		if(currentLevel==0)
		{
			pbx_OK();
		}
#endif

		if(currentLevel == lastLevel )
		{/* with same level in this 50ms */
			if((bytecnt&0x0f) > 0x0E )
			{/* no level change for about more than 550ms, no busy tone : recount it */
//				busyNum[i] = 0;
				bytecnt = (currentLevel<<4);
			}
			else
			{/* count continued */
			}
			bytecnt++;/* bytecnt low nibble *50ms = voltage not change time */
		}
		else
		{
//			bytecnt ^= 0x10;		/* XOR */
#if 0
			pbx_OK();
#endif
			if( ((bytecnt & 0x0f) >5 ) && ((bytecnt & 0x0f) < 13 ) )
			{/* level change within 400~550 ms, so no busy tone */
				busyNum[i]++;
			}
			else
			{
				busyNum[i]=0;
			}
			
			bytecnt = (currentLevel<<4);	/*save current level */
		}
		
		if(busyNum[i] > 2)	/* 19 */
		{
			busyNum[i]=0;
			bytecnt = 0x00;
			/* notify other part */
			cutresult |= (0x01 << i);

//			 	pbx_2_beep();
		}
		
		*cutbuf0 = bytecnt;

#if 0		
		if (!currentLevel )
		{
//			if (status[i+8][0]==31 || status[i+8][0]==32||status[i+8][0]==37|| status[i+8][0]==38)
			{
				TrackVoice_jishi[i]=(unsigned int )5*60*20; /* 5 minutes */
			}
		}
		else
		{
//			if (TrackVoice_jishi[i] && (status[i+8][0]==31 || status[i+8][0]==32||status[i+8][0]==37|| status[i+8][0]==38) )
			{
//				if (--TrackVoice_jishi[i]==0)
					cutresult |= (0x01 << i);
			
			}
//			else
			{
				TrackVoice_jishi[i]=0;
			}
		}
#endif

		cutbuf0++;
	}
}
#endif

#endif

#if 0
/* only for external lines */
void	__only_20s(INT8U j)
{
	INT8U data k;
	
	if(status[j][0]==32)
	{
		k=status[j][1];
		if(phoneCallLengthes[k]&&k<8&&status[k][2]==0xff)
		{
			pbx_voice_channel_broken(j,k);
			status[j][0]=43;
			status[j][2]=32;//保存原通话状态
			START_CGY(k);
			status[k][0]=04;
		}
	}
}

void	_is_only_20s()
{
	INT8U data j;
	for(j=8;j<12;j++)
	{
		if(deviceStatus[j].timeout==200)	//是否最后20s
			only_20s(j);
	}
}
#endif

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


void	check_ext_hook_event(void)
{
	INT8U	data extStates, q;
	INT8U	data m=0x01;

	q = pbx_read_hw_status(HW_STATUS_EXT_HOOK);
	extStates = (q^pbxHwStatus.extHookStatus)&(~pbxHwStatus.extRelayCtrl);
	if(extStates == 0)
	{/* No hook status has been changed this time */
		return;
	}
	pbxHwStatus.extHookStatus = q;
	
	for(q=0;q<PBX_FXS_COUNT;q++)
	{
		if( ((extStates&m) != 0 ) )
		{/* hook status of this EXT changed */
			if( ((pbxHwStatus.extHookStatus&m)==0 )&&((pbxHwStatus.extRelayCtrl&m)== 0) )
			{/* from High to Low */
	//			if(deviceStatus[q].status == STATE_IDLE ||
	//				deviceStatus[q].status == STATE_RINGING )//||	deviceStatus[q].status == STATE_PBX_RERING)
				{
					if(EXT_FLASH_TIMER_IS_ON(q) )
					{
						EXT_FLASH_TIMER_CANCEL(q);
						DEVICE_EVENT_EMIT(q, PBX_EVENT_FLASH);
						uart_debug_2_int_value(UART_DEBUG_EVENT, q, PBX_EVENT_FLASH);
					}
					else
					{
						DEVICE_EVENT_EMIT(q, PBX_EVENT_OFFHOOK);
						uart_debug_2_int_value(UART_DEBUG_EVENT, q, PBX_EVENT_OFFHOOK);
					}
				}
			}
			else if( (pbxHwStatus.extRelayCtrl&m)== 0 )
			{/* Hook status of this EXT : from Low level to High level */
	//			if(deviceStatus[q].status != STATE_IDLE && 
	//				deviceStatus[q].status != STATE_RINGING )
				{
					if(! EXT_FLASH_TIMER_IS_ON(q) )
					{
						EXT_FLASH_TIMER_SETUP(q);
					}
				}
			}
		}
		m=m<<1;
	}	
	
}


/* all timer tasks */
void	pbx_timer_task(void)
{
	INT8U data i;

	if(bool25msFlag)
	{
		bool25msFlag = FALSE;
		pbx_read_dtmf_digits();
	}
		
	if(bool50msFlag)
	{
		bool50msFlag = FALSE;
		
//		_busy_tone_detect();

		if(buzzerTimeout > 0)
		{
			buzzerTimeout--;
			if(buzzerTimeout == 0)
			{
				pbx_buzzer_ctrl(FALSE);
			}
		}
		
		for(i=0; i< PBX_FXS_COUNT; i++)
		{
			if( ringerTimer[i] >0 )
			{
				ringerTimer[i] --;
				if( ringerTimer[i] <= 0 )
				{/* This is not a FLASH event when in state of IN_CALL */
					if(PBX_CHECK_FLAGS(ringerOffPending,i) )
					{
						PBX_CLEAR_FLAGS(pbxHwStatus.extRelayCtrl, i);
						PBX_CLEAR_FLAGS(ringerOffPending, i);
					}
					else
					{
						DEVICE_EVENT_EMIT(i, PBX_EVENT_ONHOOK);
						uart_debug_2_int_value(UART_DEBUG_EVENT, i, PBX_EVENT_ONHOOK);
					}
					EXT_FLASH_TIMER_CANCEL(i);
				}
			}
		}


		if(ipGetConfigIndex!= PBX_VALUE_INVALIDATE)/* IP want to retrieve all PBX configuration data */
			net_service_config();
	}
	
	if(bool100msFlag)
	{
		bool100msFlag = FALSE;
		_account_timeout();

		check_ext_hook_event();
		check_pstn_hook_event();

		for(i=0; i<sizeof(deviceStatus)/sizeof(device_status_t); i++)
		{
			if(deviceStatus[i].timeout > 0)
			{
				deviceStatus[i].timeout--;
				if(deviceStatus[i].timeout <= 0)
				{
					DEVICE_EVENT_EMIT(i, PBX_EVENT_TIMEOUT);
				}
			}
		}

#if 0
		_is_only_20s();
		time_end1();
#endif

		for(i=0; i<ENCODE_CHANNEL_NUMBER; i++)
		{
			if( (encodingChannels[i].timeout) > 0 && (encodingChannels[i].owner != PBX_VALUE_INVALIDATE) )
			{
				encodingChannels[i].timeout--;
				if(encodingChannels[i].timeout == 0)
				{
					callerid_play_step(i);
				}
			}
		}
		callerIdSendFlags=!callerIdSendFlags;

	}
	
	if(bool400msFlag)
	{
		bool400msFlag = FALSE;
		WATCHDOG_FEED();
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

