/*
* $Id: stateMachine.c,v 1.6 2007/08/11 19:27:01 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"


code transition_t	idleStates[] =
{
	{/* this state is only for SELFTEST */
		PBX_EVENT_DIGIT,
		ext_idle_state_digits,
	},
	{
		PBX_EVENT_OFFHOOK,
		ext_idle_state_offhook,
	},
	{
		PBX_EVENT_TIMEOUT,
		ext_idle_state_timeout,
	},
	{
		PBX_SIGNAL_RINGING,
		ext_idle_state_ringing,
	},
	{
		PBX_EVENT_RINGER_ON,
		ext_idle_state_ringeron,
	},
	{
		PBX_EVENT_RINGER_OFF,
		ext_idle_state_ringeroff,
	},
	{
		PBX_EVENT_CAR,
		ext_idle_state_car,
	},
};

code transition_t	dialingStates[] =
{
	{
		PBX_EVENT_DIGIT,
		ext_dialing_state_digits,
	},
	{
		PBX_EVENT_ONHOOK,
		ext_dialing_state_onhook,
	},
	{
		PBX_EVENT_FLASH,
		ext_offhook_state_flash,
	},
	{
		PBX_EVENT_TIMEOUT,
		ext_dialing_state_timeout,
	},
	{
		PBX_SIGNAL_RINGING,
		ext_dialing_state_ringing,
	},
	{
		PBX_SIGNAL_OK,
		ext_calling_state_OK,
	},
};

#if 0
code transition_t	redialingStates[] =
{
	{
		PBX_EVENT_ONHOOK,
		ext_redialing_state_onhook,
	},
	{
		PBX_EVENT_TIMEOUT,
		ext_redialing_state_timeout,
	},
};
#endif

code transition_t	tryingStates[] =
{
	{
		PBX_EVENT_TIMEOUT,
		ext_trying_state_timeout,
	},
	{
		PBX_EVENT_ONHOOK,
		ext_trying_state_onhook,
	},

	{
		PBX_EVENT_FLASH,/* ringer off of peer */
		ext_trying_state_flash,
	},

	{
		PBX_EVENT_FLASH,
		ext_offhook_state_flash,
	},
	{
		PBX_SIGNAL_OK,
		ext_calling_state_OK,
	},
};

code transition_t	carStates[] =
{
	{
		PBX_EVENT_TIMEOUT,
		ext_car_state_timeout,
	},
	{
		PBX_EVENT_OFFHOOK,
		ext_car_state_offhook,
	},
	{
		PBX_EVENT_ONHOOK,
		ext_car_state_onhook,
	},
	{
		PBX_SIGNAL_CANCEL,
		ext_ringing_state_cancel,
	},
};

code transition_t	ringingStates[] =
{
	{
		PBX_EVENT_TIMEOUT,
		ext_ringing_state_timeout,
	},
	{
		PBX_EVENT_OFFHOOK,
		ext_ringing_state_offhook,
	},
	{
		PBX_SIGNAL_CANCEL,
		ext_ringing_state_cancel,
	},
	{
		PBX_EVENT_RINGER_OFF,
		ext_ringing_state_ringer_off,
	},
};

code transition_t	incallStates[] =
{
	{
		PBX_EVENT_TIMEOUT,
		ext_incall_state_timeout,
	},
	{
		PBX_EVENT_ONHOOK,
		ext_incall_state_onhook,
	},
	{
		PBX_EVENT_FLASH,
		ext_incall_state_flash,
	},
	{
		PBX_SIGNAL_CANCEL,
		ext_incall_state_cancel,
	},
};

code transition_t	busyStates[] =
{
	{
		PBX_EVENT_TIMEOUT,
		ext_busy_state_timeout,
	},
	{
		PBX_EVENT_ONHOOK,
		ext_busy_state_onhook,
	},
	{
		PBX_EVENT_FLASH,
		ext_offhook_state_flash,
	},
};

code transition_t	waitStates[] =
{
	{
		PBX_EVENT_OFFHOOK,
		ext_wait_state_offhook,
	},
	{
		PBX_EVENT_ONHOOK,
		ext_wait_state_onhook,
	},
	{
		PBX_SIGNAL_CANCEL,
		ext_wait_state_cancel,
	},
	{
		PBX_SIGNAL_OK,
		ext_wait_state_ok,
	},
	{
		PBX_EVENT_TIMEOUT,
		ext_wait_state_timeout,
	},
};


code transition_t	programStates[] =
{
	{
		PBX_EVENT_DIGIT,
		ext_programing_state_digits,
	},
	{
		PBX_EVENT_TIMEOUT,
		ext_programing_state_timeout,
	},
	{
		PBX_EVENT_ONHOOK,
		ext_programing_state_onhook,
	},
};

code transition_t	reringStates[] =
{
	{
		PBX_EVENT_OFFHOOK,
		ext_ringing_state_offhook,
	},
	{
		PBX_EVENT_TIMEOUT,
		ext_ringing_state_timeout,
	},
	{
		PBX_SIGNAL_CANCEL,
		ext_ringing_state_cancel,
	},
	{
		PBX_EVENT_ONHOOK,
		ext_rering_state_onhook,
	},
	{
		PBX_EVENT_RINGER_OFF,
		ext_ringing_state_ringer_off,
	},
};


code statemachine_t	pbxStateMachine[] =
{
	{
		PBX_STATE_IDLE,
		sizeof(idleStates)/sizeof(transition_t),
		idleStates,
		ext_idle_state_enter,
	},
	{
		PBX_STATE_DIALING,
		sizeof(dialingStates)/sizeof(transition_t),
		dialingStates,
		ext_dialing_state_enter,
	},
#if 0	
	{
		PBX_STATE_RE_DIALING,
		sizeof(redialingStates)/sizeof(transition_t),
		redialingStates,
		ext_redialing_state_enter,
	},
#endif	
	{
		PBX_STATE_TRYING,
		sizeof(tryingStates)/sizeof(transition_t),
		tryingStates,
		ext_trying_state_enter,
	},

	{
		PBX_STATE_CAR,
		sizeof(carStates)/sizeof(transition_t),
		carStates,
		ext_car_state_enter,
	},
	{
		PBX_STATE_RINGING,
		sizeof(ringingStates)/sizeof(transition_t),
		ringingStates,
		ext_ringing_state_enter,
	},
	
	{
		PBX_STATE_IN_CALL,
		sizeof(incallStates)/sizeof(transition_t),
		incallStates,
		ext_incall_state_enter,
	},

	{
		PBX_STATE_BUSY,
		sizeof(busyStates)/sizeof(transition_t),
		busyStates,
		ext_busy_state_enter,
	},
	
	{
		PBX_STATE_WAIT,
		sizeof(waitStates)/sizeof(transition_t),
		waitStates,
		ext_wait_state_enter,
	},
	
	{
		PBX_STATE_PROGRAM,
		sizeof(programStates)/sizeof(transition_t),
		programStates,
		ext_programing_state_enter,
	},
	{
		PBX_STATE_PBX_RERING,
		sizeof(reringStates)/sizeof(transition_t),
		reringStates,
		ext_ringing_state_enter,
	},
};

#if 0
void	_pbx_dev_handle(void)
{
	INT8U data	i;
	
	for(i = 0; i< sizeof(callDevices)/sizeof(pbx_call_device_t); i++)
	{
		if(callDevices[i].devEvent == DEV_EVENT_UNKNOWN)
			continue;
		
		if(IS_EXT_DEVICE(i) )
			devExtEventHandler(i, callDevices[i].devEvent);
		else if(IS_PSTN_LINE_DEVICE(i) )
			devPstnEventHandler(i, callDevices[i].devEvent);
		if(IS_IP_DEVICE(i) )
			devIpEventHandler(i, callDevices[i].devEvent);

		callDevices[i].devEvent = DEV_EVENT_UNKNOWN;
	}
}
#else
void	pbx_dev_handle(INT8U i, INT8U event)
{
		
	if(IS_EXT_DEVICE(i) )
		devExtEventHandler(i, event);
	else if(IS_PSTN_LINE_DEVICE(i) )
		devPstnEventHandler(i, event);
	if(IS_IP_DEVICE(i) )
		devIpEventHandler(i, event);

}
#endif

INT8U _pbx_state_handle(statemachine_t code *sm, INT8U devIndex)
{
	INT8U data	i, res;
	transition_t code *handle = sm->eventHandlers;
	
	for(i=0; i< sm->size; i++)
	{
		if(callDevices[devIndex].currentEvent == handle->event )
		{
			callDevices[devIndex].currentEvent = PBX_EVENT_UNKNOWN;
			res =  (handle->handle)(devIndex);
			
//			_pbx_dev_handle();
			
			return res;
		}
		
		handle++;
	}
	
	/* clear this unknown event */
	callDevices[devIndex].currentEvent = PBX_EVENT_UNKNOWN;

	return PBX_STATE_CONTINUE;
}

void	pbx_statemachine_handle(void)
{
	INT8U	data i, j;
	INT8U	data newState = PBX_STATE_CONTINUE;

	for(i = 0; i< sizeof(callDevices)/sizeof(pbx_call_device_t); i++)
	{
		if(callDevices[i].currentEvent == PBX_EVENT_UNKNOWN)
			continue;

		for(j=0; j< sizeof(pbxStateMachine)/sizeof(statemachine_t); j++)
		{
			if(pbxStateMachine[j].state == callDevices[i].status )
			{
				newState = _pbx_state_handle( &pbxStateMachine[j], i);
				break;
			}
		}
		
		if(newState != PBX_STATE_CONTINUE && newState != callDevices[i].status )
		{
			for(j=0; j< sizeof(pbxStateMachine)/sizeof(statemachine_t); j++)
			{
				if(pbxStateMachine[j].state == newState )
				{
					/* clean old timer for this device and new timer is set in enter_handle */
					CALL_TIMER_START(i,  TIMEOUT_NEVER);
					
					if(pbxStateMachine[j].enter_handle!= NULL)
					{
						(pbxStateMachine[j].enter_handle)(i);
			
//						_pbx_dev_handle();
					}

#if WITH_UART_DEBUG_STATE
					uart_debug_3_int_value(UART_DEBUG_STATE, i, callDevices[i].status, newState);
#endif	
					callDevices[i].status = newState;

					break;
				}
			}
		
		}
	}

//	_pbx_dev_handle();
}

