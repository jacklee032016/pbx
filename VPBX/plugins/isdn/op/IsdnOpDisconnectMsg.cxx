/*
 * $Id: IsdnOpDisconnectMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpDisconnectMsg::process( const Sptr <PbxEvent> event )
{
	if( checkIsdnMsgType( event) != ISDN_TYPE_DISCONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();

	/* CAUSE IE and codeset 6 can be both provided in DISCONNECT msg 	*/

	if ( ep->getState()->getStateType() < ISDN_EP_STATE_CONNECT )
	{
		sendCallSignal(event, CALL_RELEASE);
	}
	else
	{/* in the state of CONNECT */
	/* 1 : send DisConnect call Event to callSchduler (and callScheduler should clean this callInfo after rx this event )*/
		sendCallSignal(event, CALL_DISCONNECT);
	}
	
	/* 2 : send a release msg to ISDN device and enter into DISCONNECT_IN state to wait release_complete msg */
	Sptr <MsgRelease> relMsg = new MsgRelease( getIsdnCtrlDevice(event) );
	relMsg->sendOut( ep);

	if ( ep->getState()->getStateType() < ISDN_EP_STATE_CONNECT )
	{/* audio channel has not activate, so enter IN_DISCONNECT directly */
		return lookupEPState(event, ISDN_EP_STATE_IN_DISCONNECT );
	}
	else
	{
		/* wait CallReleaseComplete and than deactivate audio channel*/
		return PBX_STATE_DONE_WITH_EVENT;
	}
}

