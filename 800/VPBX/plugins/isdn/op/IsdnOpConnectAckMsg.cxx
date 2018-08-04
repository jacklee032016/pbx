/*
 * $Id: IsdnOpConnectAckMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpConnectAckMsg::process( const Sptr <PbxEvent> event )
{
	if(checkIsdnMsgType(event) != ISDN_TYPE_CONNECT_ACK )
	{
		return PBX_STATE_CONTINUE;
	}

	/* No IE is provided in CONNECT_ACK msg 	*/

	Sptr <EndPoint> ep = event->getEndPoint();
#if 0
	/* 1 : forward CONNECT callEvent to CallScheduler */
	CallEvent *callEvent = new CallEvent();
 	callEvent->setCallMsg( new _CallConnect() );
	callEvent->setCallId( ep->getCallId() );
	callEvent->setEndPoint( ep);

	PbxAgent::instance()->getScheduler()->reportEvent(callEvent);

	/* 2 : reply CONNECT_ACK to network side */
	Sptr <MsgConnectAck> connAckMsg = new MsgConnectAck(); 		/* No IE is added in this ISDN msg */
	pbxMsg->setIsdnMsg( connAckMsg);
#endif

#if 0
	Sptr <AudioChannel> bChan = ep->getAudio();
	assert(bChan != 0);
	bChan->activate();
#endif

	return lookupEPState(event, ISDN_EP_STATE_CONNECT);
}

