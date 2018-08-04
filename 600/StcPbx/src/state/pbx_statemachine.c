/*
* $Id: pbx_statemachine.c,v 1.12 2007/07/25 17:56:26 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

#include "_pbx_state_handles.h"

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
		ext_trying_state_OK,
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
};


code statemachine_t	pbxStateMachine[] =
{
	{
		STATE_IDLE,
		sizeof(idleStates)/sizeof(transition_t),
		idleStates,
		ext_idle_state_enter,
	},
	{
		STATE_DIALING,
		sizeof(dialingStates)/sizeof(transition_t),
		dialingStates,
		ext_dialing_state_enter,
	},
#if 0	
	{
		STATE_RE_DIALING,
		sizeof(redialingStates)/sizeof(transition_t),
		redialingStates,
		ext_redialing_state_enter,
	},
#endif	
	{
		STATE_TRYING,
		sizeof(tryingStates)/sizeof(transition_t),
		tryingStates,
		ext_trying_state_enter,
	},

	{
		STATE_RINGING,
		sizeof(ringingStates)/sizeof(transition_t),
		ringingStates,
		ext_ringing_state_enter,
	},
	
	{
		STATE_IN_CALL,
		sizeof(incallStates)/sizeof(transition_t),
		incallStates,
		ext_incall_state_enter,
	},

	{
		STATE_BUSY,
		sizeof(busyStates)/sizeof(transition_t),
		busyStates,
		ext_busy_state_enter,
	},
	
	{
		STATE_WAIT,
		sizeof(waitStates)/sizeof(transition_t),
		waitStates,
		ext_wait_state_enter,
	},
	
	{
		STATE_PBX_PROGRAM,
		sizeof(programStates)/sizeof(transition_t),
		programStates,
		ext_programing_state_enter,
	},
	{
		STATE_PBX_RERING,
		sizeof(reringStates)/sizeof(transition_t),
		reringStates,
		ext_ringing_state_enter,
	},
};

INT8U _pbx_state_handle(statemachine_t code *sm, INT8U devIndex)
{
	INT8U data	i;
	transition_t code *handle = sm->eventHandlers;
	
	for(i=0; i< sm->size; i++)
	{
		if(deviceStatus[devIndex].currentEvent == handle->event )
		{
			deviceStatus[devIndex].currentEvent = PBX_EVENT_UNKNOWN;
			return (handle->handle)(devIndex);
		}
		
		handle++;
	}
	
//	extension_ringback_tone_play(devIndex, TRUE);

//	pbx_2_beep();

	/* clear this unknown event */
	deviceStatus[devIndex].currentEvent = PBX_EVENT_UNKNOWN;

	return STATE_CONTINUE;
}

void	pbx_statemachine_handle(void)
{
	INT8U	data i, j;
	INT8U	data newState = STATE_CONTINUE;

	for(i = 0; i< sizeof(deviceStatus)/sizeof(device_status_t); i++)
	{
		if(deviceStatus[i].currentEvent == PBX_EVENT_UNKNOWN)
			continue;

		for(j=0; j< sizeof(pbxStateMachine)/sizeof(statemachine_t); j++)
		{
			if(pbxStateMachine[j].state == deviceStatus[i].status )
			{
				newState = _pbx_state_handle( &pbxStateMachine[j], i);
				break;
			}
		}
		
		if(newState != STATE_CONTINUE && newState != deviceStatus[i].status )
		{
			for(j=0; j< sizeof(pbxStateMachine)/sizeof(statemachine_t); j++)
			{
				if(pbxStateMachine[j].state == newState )
				{
					/* clean old timer for this device and new timer is set in enter_handle */
					deviceStatus[i].timeout = 0;
					
					if(pbxStateMachine[j].enter_handle!= NULL)
						(pbxStateMachine[j].enter_handle)(i);
					
					deviceStatus[i].status = newState;

//					pbx_OK();
					break;
				}
			}
		
		}
	}

}

