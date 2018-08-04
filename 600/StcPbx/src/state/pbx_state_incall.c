/*
* $Id: pbx_state_incall.c,v 1.9 2007/07/22 20:48:41 lizhijie Exp $
*/
/*
* In INCALL state, onhook, timeout event and cancel signal(Onhook by peer) are process.
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

INT8U	ext_incall_state_onhook(INT8U devIndex)
{/* start a timer to wait quit */
	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			pbx_conference_master_onhook(devIndex);
		}
		else if(IS_CONFERENCE_MASTER(deviceStatus[devIndex].peerId) )
		{
			CONFERENCE_DEL_MEMBER(devIndex);
			
			DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
		}
		
		/******* following is for CallWait/CallHold **********/
		else if(EXT_IS_HOLDER(devIndex) )
		{/* I am Call Holder */

			DEVICE_EVENT_EMIT(holdId[devIndex], PBX_SIGNAL_CANCEL);
			if( holdId[devIndex] == deviceStatus[devIndex].peerId)
			{/* holder <--> beginer */
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].flashPeerId, PBX_SIGNAL_CANCEL);
			}
			else
			{/* holder <--> primitive peer */
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
			}

			return STATE_IDLE;
		}
		else if(EXT_IS_HOLDER(deviceStatus[devIndex].peerId) )
		{/* I am peer of Holder : beginer or primitive peer of holder */
			INT8U data waiter;

			if( holdId[deviceStatus[devIndex].peerId] == devIndex )
			{/* Beginner <--> holder */
				waiter = deviceStatus[deviceStatus[devIndex].peerId].flashPeerId;
			}
			else
			{/* primitive peer <--> holder */
				waiter = holdId[deviceStatus[devIndex].peerId];
			}

			if((waiter != PBX_VALUE_INVALIDATE) && 
				(deviceStatus[waiter].status == STATE_WAIT) )
			{
				deviceStatus[deviceStatus[devIndex].peerId].peerId = waiter;
				DEVICE_EVENT_EMIT(waiter, PBX_EVENT_OFFHOOK);
			}
			else
			{/* no waiter is available, so made peer(holder) into busy state */
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
			}

			return STATE_IDLE;
		}
	
		/******* following is for Call Transfer **********/
		else if( (EXT_FLASH_IS_ON(devIndex) ||(EXT_FLASH_IS_ON(deviceStatus[devIndex].peerId))) )
		{/*
		  * case 1 : 3 parties call can be onhook 
		  * case 2,3 : both side (Flasher and the third party) can be ONHOOK firstly 
		  */

			/* case 1 : broken when 3 parties call is ON */
			if( EXT_FLASH_IS_ON(devIndex) && EXT_FLASH_IS_ON(deviceStatus[devIndex].peerId) )
			{/* this is first onhook when 3 parties call is on */

				deviceStatus[deviceStatus[devIndex].peerId].flashPeerId = PBX_VALUE_INVALIDATE;
				deviceStatus[deviceStatus[devIndex].flashPeerId].flashPeerId = PBX_VALUE_INVALIDATE;
				deviceStatus[deviceStatus[devIndex].peerId].peerId = deviceStatus[devIndex].flashPeerId;
				deviceStatus[deviceStatus[devIndex].flashPeerId].peerId = deviceStatus[devIndex].peerId;

				pbx_voice_channel_broken(devIndex, deviceStatus[devIndex].peerId);
				pbx_voice_channel_broken(devIndex, deviceStatus[devIndex].flashPeerId);

				pbx_voice_channel_connect(deviceStatus[devIndex].peerId, deviceStatus[devIndex].flashPeerId);

				return STATE_IDLE;
			}

			/* case 2 : Flasher first ONHOOK */
			if(EXT_FLASH_IS_ON(devIndex) )
			{
				deviceStatus[deviceStatus[devIndex].peerId].peerId = deviceStatus[devIndex].flashPeerId;
				deviceStatus[deviceStatus[devIndex].flashPeerId].peerId = deviceStatus[devIndex].peerId;
				
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].flashPeerId, PBX_EVENT_OFFHOOK);
				return STATE_IDLE;
			}

			/* case 3 : the third party is first ONHOOK */
			deviceStatus[deviceStatus[devIndex].peerId].peerId = deviceStatus[deviceStatus[devIndex].peerId].flashPeerId;
			deviceStatus[deviceStatus[devIndex].peerId].flashPeerId = PBX_VALUE_INVALIDATE;

			/* send offhook to the waiter */
			DEVICE_EVENT_EMIT(deviceStatus[deviceStatus[devIndex].peerId].peerId, PBX_EVENT_OFFHOOK);
			return STATE_IDLE;
		}
		else
			DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);

//		return STATE_IDLE;
	}
	
	pbx_voice_channel_broken(devIndex, deviceStatus[devIndex].peerId);

	DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
	
	return STATE_IDLE;
}

INT8U	ext_incall_state_flash(INT8U devIndex)
{

	if(IS_EXT_DEVICE(devIndex) )
	{/* this is a real flash event which can only for EXTs */

		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			if(CONFERENCE_VOICE_IS_ON(devIndex) )
			{/* voice stage of conference, ignore Flash event  */
				return STATE_CONTINUE;
			}

			CONFERENCE_ADD_MEMBER(deviceStatus[devIndex].peerId);
			DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_EVENT_FLASH);
			deviceStatus[devIndex].peerId = PBX_VALUE_INVALIDATE;

			decodingchannel_allocate(devIndex);

			return STATE_DIALING;
		}
		else if(IS_CONFERENCE_MASTER(deviceStatus[devIndex].peerId) )
		{
			if(CONFERENCE_VOICE_IS_ON(deviceStatus[devIndex].peerId)/* stage of voice */ || 
				(!IS_CONFERENCE_MEMBER(devIndex) )/* flash pressed by Peer of master */ )
			{
				return STATE_CONTINUE;
			}

			/* virtaul Flash event send by master */
			return STATE_WAIT;
		}

		else if(EXT_IS_HOLDER(devIndex) )
		{/* I am Call Holder */
			if( holdId[devIndex] == deviceStatus[devIndex].peerId)
			{/* holder <--> beginer */
			

				if(deviceStatus[deviceStatus[devIndex].flashPeerId].status == STATE_WAIT )
				{/* primitive peer which is still in state of WAIT */
				
					/* made beginer into BUSY state : terminate call with beginner */				
					DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);

					/* made primitive peer actiavted now */
					deviceStatus[devIndex].peerId = deviceStatus[devIndex].flashPeerId;
					DEVICE_EVENT_EMIT(deviceStatus[devIndex].flashPeerId, PBX_EVENT_OFFHOOK);
					/* ignore this event */
					return STATE_CONTINUE;
				}
#if 0				
				/* terminate this call */
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
				return STATE_BUSY;
#else
				
				return STATE_CONTINUE;
#endif
			}
			else
			{/* holder <--> primitive peer */
				if(deviceStatus[devIndex].flashPeerId == PBX_VALUE_INVALIDATE)
				{/* this is the first offhook event */
					if(deviceStatus[holdId[devIndex]].status == STATE_WAIT)
					{/* beginer is still waiting */
						/* send a virtual flash event to primitive peer and made it into wait state */
						DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_EVENT_FLASH);
					}
					return STATE_CONTINUE;
				}

				/* now beginner has been ONHOOK */
#if 0				
				/* terminate this call */
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
				return STATE_BUSY;
#else
				/* ignore this flash event */
				return STATE_CONTINUE;
#endif
				
			}
		}
		else if(EXT_IS_HOLDER(deviceStatus[devIndex].peerId) )
		{/* I am peer of Holder : beginer or primitive peer of holder */

			if( holdId[deviceStatus[devIndex].peerId] != devIndex )
			{/* primitive peer <--> holder */
				/* made beginer as peer of holder */
				deviceStatus[deviceStatus[devIndex].peerId].peerId = holdId[deviceStatus[devIndex].peerId] ;
				/* made i am flashPeer of Holder */
				deviceStatus[deviceStatus[devIndex].peerId].flashPeerId = devIndex;
				/* notify beginer into Call State to connect voice channel */
				DEVICE_EVENT_EMIT(holdId[deviceStatus[devIndex].peerId], PBX_EVENT_OFFHOOK);
				return STATE_WAIT;
			}
			
			/* ignore this flash event */
			return STATE_CONTINUE;
		}
		else if(EXT_FLASH_IS_OFF(devIndex) && EXT_FLASH_IS_OFF(deviceStatus[devIndex].peerId))
		{/* this is the first Flash event : I and my peer are all in state of flash off */
			if(!decodingchannel_allocate(devIndex))
			{/* No DTMF decoding channel is available, so ignore this flash waiting next flash event */
				return  STATE_CONTINUE;
			}
			
			/* send a virtual Flash event to peer made it into WAIT state */
			DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_EVENT_FLASH);
			EXT_FLASH_ENTER(devIndex);
		
			return STATE_DIALING;
		}
		else if(EXT_FLASH_IS_OK(devIndex) ||EXT_FLASH_IS_OK(deviceStatus[devIndex].peerId) )
		{
		/* when Flasher and the third party is in call state, this flash event 
		* pressed by any one is for 3 parties call */
			INT8U data flasher;
//			INT8U data third;
		
			if(EXT_FLASH_IS_OK(devIndex))
			{
				flasher = devIndex;
//				third = deviceStatus[devIndex].peerId;
			}
			else
			{
				flasher = deviceStatus[devIndex].peerId;
//				third = devIndex;
			}
		
			/* made Waiter know who is the third party*/
			deviceStatus[deviceStatus[flasher].flashPeerId].flashPeerId = deviceStatus[devIndex].peerId;
			/* made the third party know who is the Waiter */
			deviceStatus[deviceStatus[flasher].peerId].flashPeerId = deviceStatus[devIndex].flashPeerId;

			/* notify Waiter reconnect voice channel to our call */
			DEVICE_EVENT_EMIT(deviceStatus[flasher].flashPeerId, PBX_EVENT_OFFHOOK);
			return STATE_CONTINUE;
			
		}
#if 1		
		else
		{/* This is a virtual Flash event to EXT  */
			return STATE_WAIT;
		}
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex) )
	{/* after simluate offhook of PSTN, a flash event sent by HW */
		return STATE_CONTINUE;
	}
#else
	}
#endif
	return STATE_WAIT;

}


/* for both ringing timeout and ring ON-OFF timeout event */
INT8U	ext_incall_state_timeout(INT8U devIndex)
{
	if( IS_PSTN_LINE_DEVICE(devIndex))
	{/* delay for PSTN onhook action and offhook detect after rx CANCEL signal */
		PSTN_SET_FREE(devIndex);
	}


	return STATE_IDLE;//ext_incall_state_onhook(devIndex);
}

INT8U	ext_incall_state_cancel(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{/* start a timer about 500ms and delay for next offhook check */
		pbx_line_free_channel( devIndex);
		return STATE_CONTINUE;
	}
	if(IS_IP_DEVICE(devIndex) )
	{
		ip_call_tx_onhook_signal(devIndex);
		/* free other resources */

		return STATE_IDLE;
	}

#if 0
	if(IS_EXT_DEVICE(devIndex))
	{/* this is just for op of recall */
		DISCONNECT(devIndex, CHANNEL_COL_MUSIC);
	}
#endif

	if(IS_CONFERENCE_MASTER(devIndex) )
	{
		if(CONFERENCE_VOICE_IS_ON(devIndex) &&
			CONFERENCE_HAS_SOME_MEMBER(devIndex)  )
		{/* voice stage of conference and has some slave, so master keep in this state */
			return STATE_CONTINUE;
		}

	}

	/* EXT start playing busy tone */
	return STATE_BUSY;
}


void ext_incall_state_enter(INT8U devIndex)
{
	deviceStatus[devIndex].timeout = 0;
	/* EXT to EXT */
	if(deviceStatus[devIndex].status == STATE_TRYING)
	{/*as caller */
	}
	else
	{/* as callee */
	}

	/* EXT to PSTN Line */
	if(deviceStatus[devIndex].status == STATE_DIALING)
	{/*as caller : EXT */
		deviceStatus[devIndex].timeout = 0;
	}
	
	return;
}

