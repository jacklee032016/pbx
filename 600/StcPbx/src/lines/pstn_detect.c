/*
* $Id: pstn_detect.c,v 1.3 2007/07/10 19:59:25 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

#if 0
void	check_pstn_hook_event(void)
{
	INT8U	data linesStates, q;

/*
	linesStates= track_ldstatus0|track_ldstatus1;//|baseConfigs.trunkStatusFlags;//pbxHwStatus.trackFreeStatus;
*/	
	RD_TRKSTATUS(&linesStates,&backsoundbuf);
	
	for(q=PBX_FXS_COUNT;q<PBX_ROW_DEVICE_COUNT;q++)
	{
		if(deviceStatus[q].status != STATE_IN_CALL )
		{
			if( (linesStates &SET_BIT( 0x01,GET_PSTN_NUMBER(q) )  )==0  )
			{/* low level */
				
				if(pstnPending[GET_PSTN_NUMBER(q)] == PSTN_RINGER_DISABLE ||
					pstnPending[GET_PSTN_NUMBER(q)] == PSTN_RINGER_OFF )
				{/* ringer begin */
					if(pstnPending[GET_PSTN_NUMBER(q)] == PSTN_RINGER_DISABLE)
					{
						deviceStatus[q].timeoutCounter = 0;
						pstnRingerOffTimer[GET_PSTN_NUMBER(q)] = 0;
						pstnRingerOnTimer[GET_PSTN_NUMBER(q)] = 0;
						/* disable first ringer ON event */
//						DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_ON);
					}
					else //if(pstnPending[GET_PSTN_NUMBER(q)] == PSTN_RINGER_OFF)
					{
						if( (pstnRingerOnTimer[GET_PSTN_NUMBER(q)] >= phoneParams[baseConfigs.signalType].ringer_on_time -3 ) &&
							(pstnRingerOnTimer[GET_PSTN_NUMBER(q)] <=  phoneParams[baseConfigs.signalType].ringer_on_time +3 ) )
						{
								deviceStatus[q].timeoutCounter++;
								DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_ON);
						}
					}
					
					uart_tx_2_int_value(CMD_UART_DEBUG, 1, pstnRingerOffTimer[GET_PSTN_NUMBER(q)] );
					pstnPending[GET_PSTN_NUMBER(q)] = PSTN_RINGER_ON;
					pstnRingerOnTimer[GET_PSTN_NUMBER(q)] = 0;
					
				}
				
				pstnRingerOnTimer[GET_PSTN_NUMBER(q)]++;
			}
			else 
			{
//					pbxBuzzer(8);
				if(pstnPending[GET_PSTN_NUMBER(q)] == PSTN_RINGER_ON )
				{/* ringer end */

					if( (pstnRingerOnTimer[GET_PSTN_NUMBER(q)] >= phoneParams[baseConfigs.signalType].ringer_on_time -3 ) &&
						(pstnRingerOnTimer[GET_PSTN_NUMBER(q)] <=  phoneParams[baseConfigs.signalType].ringer_on_time +3 ) )
					{
//						if(deviceStatus[q].timeoutCounter > 0)
						{
							deviceStatus[q].timeoutCounter++;
							DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_OFF);
						}
					}
					
					if( (pstnRingerOffTimer[GET_PSTN_NUMBER(q)] >= TIMEOUT_CAR_LENGTH-2 ) &&
						(pstnRingerOffTimer[GET_PSTN_NUMBER(q)] <=  TIMEOUT_CAR_LENGTH+1 ) &&
						(pstnRingerOnTimer[GET_PSTN_NUMBER(q)] >= TIMEOUT_CAR_LENGTH-2 ) &&
						(pstnRingerOnTimer[GET_PSTN_NUMBER(q)] <=  TIMEOUT_CAR_LENGTH+1 ) )
					{
						DEVICE_EVENT_EMIT(q, PBX_EVENT_CAR);
					}
				
					pstnPending[GET_PSTN_NUMBER(q)] = PSTN_RINGER_OFF;
					uart_tx_2_int_value(CMD_UART_DEBUG, 2, pstnRingerOnTimer[GET_PSTN_NUMBER(q)] );
					pstnRingerOffTimer[GET_PSTN_NUMBER(q)] = 0;
				}
				
				pstnRingerOffTimer[GET_PSTN_NUMBER(q)]++;

				if(pstnRingerOffTimer[GET_PSTN_NUMBER(q)] > (phoneParams[baseConfigs.signalType].ringer_off_time + 12) )
				{
//					pbxBuzzer(20);
					DEVICE_EVENT_EMIT(q, PBX_EVENT_TIMEOUT);
					pstnPending[GET_PSTN_NUMBER(q)] = PSTN_RINGER_DISABLE;
				}

			}
		}
		
		 /* after busy detected */
		 if( PBX_CHECK_FLAGS(cutresult, GET_PSTN_NUMBER(q) )  )
		 {
			DEVICE_EVENT_EMIT(q, PBX_EVENT_ONHOOK);
		 }

		 cutresult=0;
	}
	
}

/*
void	check_pstn_hook_event(void)
{
	INT8U	data linesStates, q;
	BOOL	isRinging;

	RD_TRKSTATUS(&linesStates,&backsoundbuf);
	
	for(q=PBX_FXS_COUNT;q<PBX_ROW_DEVICE_COUNT;q++)
	{
		if(deviceStatus[q].status != STATE_IN_CALL )
		{
			isRinging = ((linesStates &SET_BIT( 0x01,GET_PSTN_NUMBER(q) )  )==0)?TRUE:FALSE;

			
			if( pstnPending[GET_PSTN_NUMBER(q)] == PSTN_RINGER_DISABLE)
			{
				if(isRinging)
				{
					deviceStatus[q].timeoutCounter = 0;
					pstnRingerOffTimer[GET_PSTN_NUMBER(q)] = 0;
					pstnRingerOnTimer[GET_PSTN_NUMBER(q)] = 0;
					deviceStatus[q].timeoutCounter++;
					DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_ON);
					
					pstnPending[GET_PSTN_NUMBER(q)] = PSTN_RINGER_ON;
#if UART_DEBUG						
					uart_tx_2_int_value(CMD_UART_DEBUG, 0, pstnRingerOffTimer[GET_PSTN_NUMBER(q)] );
#endif
				}
			}
			else if( pstnPending[GET_PSTN_NUMBER(q)] == PSTN_RINGER_ON)
			{
				if(isRinging)
				{
					pstnRingerOnTimer[GET_PSTN_NUMBER(q)] ++;
				}
				else
				{
					pstnRingerOffTimer[GET_PSTN_NUMBER(q)] = 0;
				}
				
				if( (pstnRingerOnTimer[GET_PSTN_NUMBER(q)] >= phoneParams[baseConfigs.signalType].ringer_on_time -3 ) &&
					(pstnRingerOnTimer[GET_PSTN_NUMBER(q)] <=  phoneParams[baseConfigs.signalType].ringer_on_time +3 ) )
				{
//						if(deviceStatus[q].timeoutCounter > 0)
					{
						deviceStatus[q].timeoutCounter++;
						pstnRingerOnTimer[GET_PSTN_NUMBER(q)] = 0;
						DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_OFF);
					}
				}
			}	
			else if( pstnPending[GET_PSTN_NUMBER(q)] == PSTN_RINGER_OFF)
			{

				if(isRinging)
				{
					pstnRingerOnTimer[GET_PSTN_NUMBER(q)] ++;
				}
				else
				{
					pstnRingerOffTimer[GET_PSTN_NUMBER(q)]++;
				}
				
				if(pstnRingerOffTimer[GET_PSTN_NUMBER(q)] > (phoneParams[baseConfigs.signalType].ringer_off_time + 12) )
				{
					DEVICE_EVENT_EMIT(q, PBX_EVENT_TIMEOUT);
					pstnPending[GET_PSTN_NUMBER(q)] = PSTN_RINGER_DISABLE;
					pstnRingerOffTimer[GET_PSTN_NUMBER(q)] = 0;
				}
				
				if(pstnRingerOnTimer[GET_PSTN_NUMBER(q)] > 3 )
				{
					DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_ON);
					pstnRingerOffTimer[GET_PSTN_NUMBER(q)] = 0;
					pstnPending[GET_PSTN_NUMBER(q)] = PSTN_RINGER_ON;
				}
			}

		}
		
		 if( PBX_CHECK_FLAGS(cutresult, GET_PSTN_NUMBER(q) )  )
		 {
			DEVICE_EVENT_EMIT(q, PBX_EVENT_ONHOOK);
		 }

		 cutresult=0;
	}
	
}
*/

void	check_pstn_hook_event(void)
{
	INT8U	data linesStates, q, index;
	BOOL	isRinging;
	static INT8U	i[4]={0,0,0,0}, j[4] = {0, 0,0, 0};

	RD_TRKSTATUS(&linesStates,&backsoundbuf);
	
	for(q=PBX_FXS_COUNT;q<PBX_ROW_DEVICE_COUNT;q++)
	{
		if(deviceStatus[q].status != STATE_IN_CALL )
		{
			isRinging = ((linesStates &SET_BIT( 0x01,GET_PSTN_NUMBER(q) )  )==0)?TRUE:FALSE;

			index = GET_PSTN_NUMBER(q);


			if(pstnRingerTimeout[index] > 0)
				pstnRingerTimeout[index] --;

			if(pstnPending[index]!= PSTN_RINGER_DISABLE)
			{
				if(isRinging)
				{
					i[index]++;
					uart_tx_2_int_value(CMD_UART_DEBUG, 1, i[index] );
				}
				else
				{
					j[index]++;
					uart_tx_2_int_value(CMD_UART_DEBUG, 0, j[index] );
				}
			}

			switch( pstnPending[index])
			{
				case PSTN_RINGER_DISABLE:
				{
					if(isRinging)
					{
						pstnRingerCounter[index]++;
						if(pstnRingerCounter[index]>= 2)
						{
							DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_ON);
							/* this is the first ringer on event, not count it */
							deviceStatus[q].timeoutCounter = 0;

#if 0//UART_DEBUG						
							uart_tx_2_int_value(CMD_UART_DEBUG, 0, pstnRingerCounter[index] );
#endif
							PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_ON_PENDING, 3*PBX_100MS);
						}
					}
					else
					{
						pstnRingerCounter[index] = 0;
					}
					break;
				}
				case PSTN_RINGER_ON_PENDING:
				{
					if(PSTN_RINGER_IS_TIMEOUT(index) )
					{
						PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_ON_DETECT, 5*PBX_100MS);
					}
					break;
				}	
				case PSTN_RINGER_ON_DETECT:
				{
					if(!isRinging)
					{
						pstnRingerCounter[index]++;
					}

					if(PSTN_RINGER_IS_TIMEOUT(index) )
					{
#if 0//UART_DEBUG						
						uart_tx_2_int_value(CMD_UART_DEBUG, 2, pstnRingerCounter[index] );
#endif
						if(pstnRingerCounter[index] > 3)
						{
							DEVICE_EVENT_EMIT(q, PBX_EVENT_CAR);
							PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_BLOCKED, 0);
						}
						else
						{
							DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_OFF);
							deviceStatus[q].timeoutCounter++;
							PSTN_RINGER_NEW_STATE(index,PSTN_RINGER_OFF_PENDING, 5*PBX_100MS);
						}
					}
					
					break;
				}
				case PSTN_RINGER_OFF_PENDING:
				{
					if(PSTN_RINGER_IS_TIMEOUT(index) )
					{
						PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_OFF_DETECT, (phoneParams[baseConfigs.signalType].ringer_off_time ) );
					}
					break;
				}
				case PSTN_RINGER_OFF_DETECT:
				{
					if(isRinging)
					{
						pstnRingerCounter[index]++;
						if(pstnRingerCounter[index]>= 2)
						{
#if 0//UART_DEBUG						
							uart_tx_2_int_value(CMD_UART_DEBUG, 3, pstnRingerCounter[index] );
#endif
							DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_ON);
							deviceStatus[q].timeoutCounter++;
							PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_ON_PENDING, 3*PBX_100MS);
							break;
						}
					}
					else
					{
						pstnRingerCounter[index] = 0;
					}

					if(PSTN_RINGER_IS_TIMEOUT(index) )
					{
						PSTN_RINGER_NEW_STATE(index,PSTN_RINGER_DISABLE, 0);
						DEVICE_EVENT_EMIT(q, PBX_EVENT_TIMEOUT);
#if 0//UART_DEBUG						
						uart_tx_2_int_value(CMD_UART_DEBUG, 4, pstnRingerCounter[index] );
#endif
					}
					
					break;
				}
				default:
					break;
			}

		}
		
		 /* after busy detected */
		 if( PBX_CHECK_FLAGS(cutresult, GET_PSTN_NUMBER(q) )  )
		 {
			DEVICE_EVENT_EMIT(q, PBX_EVENT_ONHOOK);
		 }

		 cutresult=0;
	}
	
}
#else

void	check_pstn_hook_event(void)
{
	INT8U	data linesStates, q, index;
	BOOL	isRinging;

	linesStates = pbx_read_hw_status(HW_STATUS_PSTN_HOOK_BUSY);
	backsoundbuf = linesStates & 0X0F;
	linesStates = (linesStates>>4);
//	RD_TRKSTATUS(&linesStates,&backsoundbuf);
	
	for(q=PBX_FXS_COUNT;q<PBX_ROW_DEVICE_COUNT;q++)
	{
		if(deviceStatus[q].status != STATE_IN_CALL )
		{
			isRinging = ((linesStates &SET_BIT( 0x01,GET_PSTN_NUMBER(q) )  )==0)?TRUE:FALSE;

			index = GET_PSTN_NUMBER(q);

			if(pstnRingerTimeout[index] > 0)
				pstnRingerTimeout[index] --;

			switch( pstnPending[index])
			{
				case PSTN_RINGER_DISABLE:
				{
					if(isRinging)
					{
						pstnRingerCounter[index]++;
						if(pstnRingerCounter[index]>= 2)
						{
							/* this is the first ringer on event, not count it */
//	busucomcome = busucomcome|0x0F;//(SET_BIT(1, currentCallerIdChannel));
//	send_busycome(busucomcome);
							deviceStatus[q].timeoutCounter = 0;
							DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_ON);

#if WITH_PSTN_FLOW_DEBUG						
							uart_tx_2_int_value(CMD_UART_DEBUG, PSTN_FLOW_BEGIN_RINGER_ON, deviceStatus[q].timeoutCounter);//pstnRingerCounter[index] );
#endif
							if(LOCAL_IS_JAPAN() && PSTN_CALLID_IS_ON() )
							{/* CAR */
								PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_ON_PENDING, TIMEOUT_CAR_LENGTH*4);
							}
							else
							{
								PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_OFF_PENDING, (phoneParams[baseConfigs.signalType].ringer_on_time+PBX_100MS) );
							}
						}
					}
					else
					{
						pstnRingerCounter[index] = 0;
					}
					break;
				}
				case PSTN_RINGER_ON_PENDING:
				{
					if(PSTN_RINGER_IS_TIMEOUT(index) )
					{
						DEVICE_EVENT_EMIT(q, PBX_EVENT_CAR);
						PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_BLOCKED, (phoneParams[baseConfigs.signalType].ringer_off_time +5*PBX_100MS) );
#if WITH_PSTN_FLOW_DEBUG						
						uart_tx_2_int_value(CMD_UART_DEBUG, PSTN_FLOW_BEGIN_CAR, deviceStatus[q].timeoutCounter );
#endif
					}
					break;
				}
				case PSTN_RINGER_OFF_PENDING:
				{
					if(PSTN_RINGER_IS_TIMEOUT(index) )
					{
						deviceStatus[q].timeoutCounter++;
						DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_OFF);
						PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_OFF_DETECT, (phoneParams[baseConfigs.signalType].ringer_off_time +5*PBX_100MS) );
#if WITH_PSTN_FLOW_DEBUG						
						uart_tx_2_int_value(CMD_UART_DEBUG, PSTN_FLOW_RINGER_OFF, deviceStatus[q].timeoutCounter);
#endif
					}
					break;
				}
				case PSTN_RINGER_OFF_DETECT:
				{
					if(isRinging)
					{
						pstnRingerCounter[index]++;
						if(pstnRingerCounter[index]>= 2)
						{
							deviceStatus[q].timeoutCounter++;
							DEVICE_EVENT_EMIT(q, PBX_EVENT_RINGER_ON);
							PSTN_RINGER_NEW_STATE(index, PSTN_RINGER_OFF_PENDING, (phoneParams[baseConfigs.signalType].ringer_on_time+PBX_100MS));
#if WITH_PSTN_FLOW_DEBUG					
							uart_tx_2_int_value(CMD_UART_DEBUG, PSTN_FLOW_RINGER_ON, deviceStatus[q].timeoutCounter );
#endif
							break;
						}
					}
					else
					{
						pstnRingerCounter[index] = 0;
					}

					if(PSTN_RINGER_IS_TIMEOUT(index) )
					{
						PSTN_RINGER_NEW_STATE(index,PSTN_RINGER_DISABLE, 0);
						DEVICE_EVENT_EMIT(q, PBX_EVENT_TIMEOUT);
#if WITH_PSTN_FLOW_DEBUG					
						uart_tx_2_int_value(CMD_UART_DEBUG, PSTN_FLOW_RINGER_TIMEOUT, pstnRingerCounter[index] );
#endif
					}
					
					break;
				}
				default:
					break;
			}

		}
		
		 /* after busy detected */
		 if( PBX_CHECK_FLAGS(cutresult, GET_PSTN_NUMBER(q) )  )
		 {
			DEVICE_EVENT_EMIT(q, PBX_EVENT_ONHOOK);
		 }

		 cutresult=0;
	}
	
}

#endif

