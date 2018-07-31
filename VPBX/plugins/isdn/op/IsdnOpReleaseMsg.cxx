/*
 * $Id: IsdnOpReleaseMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpReleaseMsg::process( const Sptr <PbxEvent> event )
{
	if(checkIsdnMsgType(event) != ISDN_TYPE_RELEASE )
	{
		return PBX_STATE_CONTINUE;
	}

#if 0	
	Sptr <IeCause> cause = relMsg->getCause();
//	cause
#endif
	Sptr <EndPoint> ep = event->getEndPoint();

	if( ep->getState()->getStateType() > ISDN_EP_STATE_IDLE && ep->getState()->getStateType()<ISDN_EP_STATE_CONNECT)	
	{
		sendCallSignal(event, CALL_RELEASE);
	}
	else if( ep->getState()->getStateType() == ISDN_EP_STATE_OUT_DISCONNECT && ep->getState()->getStateType() == ISDN_EP_STATE_IN_DISCONNECT)
	{	
//		sendCallSignal(event, CALL_RELEASE);
	}	

	/* After RX RELEASE msg, RELEASE_COMPLETE msg is TX by protocol stack */
#if 0
	/* NT stack send RELEASE_COMPLATE before report RELEASE msg, so only TE send out */
	if(isdnMsg->getCtrlDevice()->getNtMode() == ISDN_DEV_TYPE_USER )
	{/* 2 : response with release_complete */
		Sptr <MsgReleaseComplete> relComMsg = new MsgReleaseComplete( isdnMsg->getCtrlDevice());
		relComMsg->sendOut(ep);
	}
#endif

	/*3 : return to IDLE state */
	return lookupEPState(event, ISDN_EP_STATE_WAIT_CLEAR_CR);
}

