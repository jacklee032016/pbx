/*
* $Id: stateIncall.c,v 1.5 2007/09/08 18:56:50 lizhijie Exp $
*/
/*
* In INCALL state, onhook, timeout event and cancel signal(Onhook by peer) are process.
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

INT8U	ext_incall_state_onhook(INT8U devIndex)
{/* start a timer to wait quit */
	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			pbx_conference_master_onhook(devIndex);
		}
		else if(IS_CONFERENCE_MASTER(CALL_MY_PEER(devIndex) ) )
		{
			CONFERENCE_DEL_MEMBER(devIndex);
			
			CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
		}
		
		/******* following is for CallWait/CallHold **********/
		else if(EXT_IS_HOLDER(devIndex) )
		{/* I am Call Holder */

			CALL_EVENT_EMIT(extDevices.holdId[devIndex], PBX_SIGNAL_CANCEL);
			if( extDevices.holdId[devIndex] == CALL_MY_PEER(devIndex) )
			{/* holder <--> beginer */
				CALL_FLASH_EVENT(devIndex, PBX_SIGNAL_CANCEL);
			}
			else
			{/* holder <--> primitive peer */
				CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
			}

			return PBX_STATE_IDLE;
		}
		else if(EXT_IS_HOLDER(CALL_MY_PEER(devIndex) ) )
		{/* I am peer of Holder : beginer or primitive peer of holder */
			INT8U data waiter;

			if( extDevices.holdId[CALL_MY_PEER(devIndex) ] == devIndex )
			{/* Beginner <--> holder */
				waiter = CALL_MY_FLASH(CALL_MY_PEER(devIndex));
			}
			else
			{/* primitive peer <--> holder */
				waiter = extDevices.holdId[CALL_MY_PEER(devIndex)];
			}

			if((waiter != PBX_VALUE_INVALIDATE) && 
				(CALL_CHECK_STATUS(waiter,PBX_STATE_WAIT) ) )
			{
				CALL_MY_PEER(CALL_MY_PEER(devIndex) ) = waiter;
				CALL_EVENT_EMIT(waiter, PBX_EVENT_OFFHOOK);
			}
			else
			{/* no waiter is available, so made peer(holder) into busy state */
				CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
			}

			return PBX_STATE_IDLE;
		}
	
		/******* following is for Call Transfer **********/
		else if( (EXT_FLASH_IS_ON(devIndex)) ||(EXT_FLASH_IS_ON(CALL_MY_PEER(devIndex) )) )
		{/*
		  * case 1 : 3 parties call can be onhook 
		  * case 2,3 : both side (Flasher and the third party) can be ONHOOK firstly 
		  */

			/* case 1 : broken when 3 parties call is ON */
			if( EXT_FLASH_IS_ON(devIndex) && EXT_FLASH_IS_ON(CALL_MY_PEER(devIndex) ) )
			{/* this is first onhook when 3 parties call is on */

				CALL_MY_FLASH(CALL_MY_PEER(devIndex)) = PBX_VALUE_INVALIDATE;
				CALL_MY_FLASH(CALL_MY_FLASH(devIndex)) = PBX_VALUE_INVALIDATE;
				CALL_MY_PEER(CALL_MY_PEER(devIndex)) = CALL_MY_FLASH(devIndex);
				CALL_MY_PEER(CALL_MY_FLASH(devIndex)) = CALL_MY_PEER(devIndex);

				voiceChannelBroken(devIndex, CALL_MY_PEER(devIndex) );
				voiceChannelBroken(devIndex, CALL_MY_FLASH(devIndex));

				voiceChannelConnect(CALL_MY_PEER(devIndex), CALL_MY_FLASH(devIndex));

				return PBX_STATE_IDLE;
			}

			/* case 2 : Flasher first ONHOOK */
			if(EXT_FLASH_IS_ON(devIndex) )
			{
				CALL_MY_PEER(CALL_MY_PEER(devIndex)) = CALL_MY_FLASH(devIndex);
				CALL_MY_PEER(CALL_MY_FLASH(devIndex)) = CALL_MY_PEER(devIndex);
				
				CALL_FLASH_EVENT(devIndex, PBX_EVENT_OFFHOOK);
				return PBX_STATE_IDLE;
			}

			/* case 3 : the third party is first ONHOOK */
			CALL_MY_PEER(CALL_MY_PEER(devIndex)) = CALL_MY_FLASH(CALL_MY_PEER(devIndex));
			CALL_MY_FLASH(CALL_MY_PEER(devIndex)) = PBX_VALUE_INVALIDATE;

			/* send offhook to the waiter */
			CALL_EVENT_EMIT(CALL_MY_PEER(CALL_MY_PEER(devIndex)), PBX_EVENT_OFFHOOK);
			return PBX_STATE_IDLE;
		}
		else
		{
			CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
		}

//		return PBX_STATE_IDLE;
	}
	
	voiceChannelBroken(devIndex,CALL_MY_PEER(devIndex) );

	CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
	
	if( IS_PSTN_LINE_DEVICE(devIndex))
		return PBX_STATE_CONTINUE;
	
	return PBX_STATE_IDLE;
}

INT8U	ext_incall_state_flash(INT8U devIndex)
{

	if(IS_EXT_DEVICE(devIndex) )
	{/* this is a real flash event which can only for EXTs */

		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			if(CONFERENCE_VOICE_IS_ON(devIndex) )
			{/* voice stage of conference, ignore Flash event  */
				return PBX_STATE_CONTINUE;
			}

			CONFERENCE_ADD_MEMBER(CALL_MY_PEER(devIndex));
			CALL_PEER_EVENT(devIndex, PBX_EVENT_FLASH);
			CALL_MY_PEER(devIndex) = PBX_VALUE_INVALIDATE;

			dtmfDecodingChannelAllocate(devIndex);

			return PBX_STATE_DIALING;
		}
		else if(IS_CONFERENCE_MASTER(CALL_MY_PEER(devIndex) ) )
		{
			if(CONFERENCE_VOICE_IS_ON(CALL_MY_PEER(devIndex) )/* stage of voice */ || 
				(!IS_CONFERENCE_MEMBER(devIndex) )/* flash pressed by Peer of master */ )
			{
				return PBX_STATE_CONTINUE;
			}

			/* virtaul Flash event send by master */
			return PBX_STATE_WAIT;
		}

		else if(EXT_IS_HOLDER(devIndex) )
		{/* I am Call Holder */
			if( extDevices.holdId[devIndex] == CALL_MY_PEER(devIndex) )
			{/* holder <--> beginer */
			

				if(CALL_CHECK_STATUS(CALL_MY_FLASH(devIndex), PBX_STATE_WAIT) )
				{/* primitive peer which is still in state of WAIT */
				
					/* made beginer into BUSY state : terminate call with beginner */				
					CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);

					/* made primitive peer actiavted now */
					CALL_MY_PEER(devIndex) = CALL_MY_FLASH(devIndex);
					CALL_FLASH_EVENT(devIndex, PBX_EVENT_OFFHOOK);
					/* ignore this event */
					return PBX_STATE_CONTINUE;
				}
#if 0				
				/* terminate this call */
				CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
				return PBX_STATE_BUSY;
#else
				
				return PBX_STATE_CONTINUE;
#endif
			}
			else
			{/* holder <--> primitive peer */
				if(CALL_MY_FLASH(devIndex) == PBX_VALUE_INVALIDATE)
				{/* this is the first offhook event */
					if(CALL_CHECK_STATUS(extDevices.holdId[devIndex], PBX_STATE_WAIT) )
					{/* beginer is still waiting */
						/* send a virtual flash event to primitive peer and made it into wait state */
						CALL_PEER_EVENT(devIndex, PBX_EVENT_FLASH);
					}
					return PBX_STATE_CONTINUE;
				}

				/* now beginner has been ONHOOK */
#if 0				
				/* terminate this call */
				CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
				return PBX_STATE_BUSY;
#else
				/* ignore this flash event */
				return PBX_STATE_CONTINUE;
#endif
				
			}
		}
		else if(EXT_IS_HOLDER(CALL_MY_PEER(devIndex) ) )
		{/* I am peer of Holder : beginer or primitive peer of holder */

			if( extDevices.holdId[CALL_MY_PEER(devIndex)] != devIndex )
			{/* primitive peer <--> holder */
				/* made beginer as peer of holder */
				CALL_MY_PEER(CALL_MY_PEER(devIndex)) = extDevices.holdId[CALL_MY_PEER(devIndex)] ;
				/* made i am flashPeer of Holder */
				CALL_MY_FLASH(CALL_MY_PEER(devIndex)) = devIndex;
				/* notify beginer into Call State to connect voice channel */
				CALL_EVENT_EMIT(extDevices.holdId[CALL_MY_PEER(devIndex)], PBX_EVENT_OFFHOOK);
				return PBX_STATE_WAIT;
			}
			
			/* ignore this flash event */
			return PBX_STATE_CONTINUE;
		}
		else if(EXT_FLASH_IS_OFF(devIndex) && EXT_FLASH_IS_OFF(CALL_MY_PEER(devIndex) ))
		{/* this is the first Flash event : I and my peer are all in state of flash off */
			if(!dtmfDecodingChannelAllocate(devIndex))
			{/* No DTMF decoding channel is available, so ignore this flash waiting next flash event */
				return  PBX_STATE_CONTINUE;
			}
			
			/* send a virtual Flash event to peer made it into WAIT state */
			CALL_PEER_EVENT(devIndex, PBX_EVENT_FLASH);
			EXT_FLASH_ENTER(devIndex);
		
			return PBX_STATE_DIALING;
		}
		else if(EXT_FLASH_IS_OK(devIndex) ||EXT_FLASH_IS_OK(CALL_MY_PEER(devIndex) ) )
		{
		/* when Flasher and the third party is in call state, this flash event 
		* pressed by any one is for 3 parties call */
			INT8U data flasher;
//			INT8U data third;
		
			if(EXT_FLASH_IS_OK(devIndex))
			{
				flasher = devIndex;
//				third = CALL_MY_PEER(devIndex);
			}
			else
			{
				flasher = CALL_MY_PEER(devIndex);
//				third = devIndex;
			}
		
			/* made Waiter know who is the third party*/
			CALL_MY_FLASH(CALL_MY_FLASH(flasher)) = CALL_MY_PEER(devIndex);
			/* made the third party know who is the Waiter */
			CALL_MY_FLASH(CALL_MY_PEER(flasher)) = CALL_MY_FLASH(devIndex);

			/* notify Waiter reconnect voice channel to our call */
			CALL_FLASH_EVENT(flasher, PBX_EVENT_OFFHOOK);
			return PBX_STATE_CONTINUE;
			
		}
#if 1		
		else
		{/* This is a virtual Flash event to EXT  */
			return PBX_STATE_WAIT;
		}
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex) &&
		EXT_FLASH_IS_OFF(CALL_MY_PEER(devIndex) ) )
	{/* after simluate offhook of PSTN, a flash event sent by HW */
		return PBX_STATE_CONTINUE;
	}

#else
	}
#endif
	return PBX_STATE_WAIT;

}


/* for both ringing timeout and ring ON-OFF timeout event */
INT8U	ext_incall_state_timeout(INT8U devIndex)
{
	if( IS_PSTN_LINE_DEVICE(devIndex))
	{/* delay for PSTN onhook action and offhook detect after rx CANCEL signal */
		pstnDeviceFree(devIndex);
	}

	return PBX_STATE_IDLE;
}

INT8U	ext_incall_state_cancel(INT8U devIndex)
{
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_CANCEL);
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{/* start a timer about 500ms and delay for next offhook check */
		return PBX_STATE_CONTINUE;
	}
	if(IS_IP_DEVICE(devIndex) )
	{
		return PBX_STATE_IDLE;
	}

	if(IS_CONFERENCE_MASTER(devIndex) )
	{
		if(CONFERENCE_VOICE_IS_ON(devIndex) &&
			CONFERENCE_HAS_SOME_MEMBER(devIndex)  )
		{/* voice stage of conference and has some slave, so master keep in this state */
			return PBX_STATE_CONTINUE;
		}
	}

	/* EXT start playing busy tone */
	return PBX_STATE_BUSY;
}


void ext_incall_state_enter(INT8U devIndex)
{
	callDevices[devIndex].timeout = 0;
	/* EXT to EXT */
	if(callDevices[devIndex].status == PBX_STATE_TRYING)
	{/*as caller */
	}
	else
	{/* as callee */
	}

	/* EXT to PSTN Line */
	if(callDevices[devIndex].status == PBX_STATE_DIALING)
	{/*as caller : EXT */
		callDevices[devIndex].timeout = 0;
	}
	
	return;
}

