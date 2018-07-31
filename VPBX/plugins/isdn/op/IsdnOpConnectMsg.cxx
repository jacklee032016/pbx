/*
 * $Id: IsdnOpConnectMsg.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "IsdnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> IsdnOpConnectMsg::process( const Sptr <PbxEvent> event )
{
	if(checkIsdnMsgType(event) != ISDN_TYPE_CONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	/* No IE is provided in CONNECT msg 	*/
	Sptr <EndPoint> ep = event->getEndPoint();
	Sptr <AudioChannel> bChan = ep->getAudio();
	assert(bChan!=0);
	bChan->activate();

	IsdnCtrlDevice *ctrl = getIsdnCtrlDevice(event);
	/* 1 : create a CONNECT callEvent and send to CallScheduler */

	sendCallSignal(event, CALL_CONNECT);

	/* 2 : reply CONNECT_ACK to network side, this is optional  */
	if( ctrl->getNtMode() == ISDN_DEV_TYPE_NET)
	{/* Network side must reply a CONNECT_ACK */
		Sptr <MsgConnectAck> connAckMsg = new MsgConnectAck( ctrl ); 		/* No IE is added in this ISDN msg */
		connAckMsg->sendOut( ep);
		//pbxMsg->setIsdnMsg( connAckMsg);
	}

	return lookupEPState(event, ISDN_EP_STATE_CONNECT);
}

