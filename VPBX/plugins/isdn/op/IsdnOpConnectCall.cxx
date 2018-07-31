/*
 * $Id: IsdnOpConnectCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpConnectCall::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_CONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> ep = event->getEndPoint();
	IsdnCtrlDevice *ctrl = getIsdnCtrlDevice(event);

	Sptr <MsgConnect> conn = new MsgConnect( ctrl );
	conn->sendOut( ep);

	Sptr <AudioChannel> bChan = ep->getAudio();
	assert(bChan != 0);
	bChan->activate();
	
	/* reply ConnectAck which make CallScheduler enter CONNECT state and activate audioThread for my audioChannel */
	sendCallSignal(event, CALL_CONNECT_ACK);

//	if( ep->getState()->getStateType() == ISDN_EP_STATE_IN_SETUP )
//		return ep->findState( ISDN_EP_STATE_IN_ALERTING);
	if(ctrl->getNtMode() == ISDN_DEV_TYPE_USER )
	{/* as a IN state, state chane into CONNECT only when RX CONNECT_ACK 
	* NT must reply with CONNECT_ACK
	*/
		return PBX_STATE_CONTINUE;
	}
	
	/* user side reply CONNECT_ACK is option, for TA no CONNECT_ACK is reply */
	
	return lookupEPState(event, ISDN_EP_STATE_CONNECT );
}

