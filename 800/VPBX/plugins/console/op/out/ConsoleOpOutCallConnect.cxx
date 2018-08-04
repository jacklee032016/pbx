/*
 * $Id: ConsoleOpOutCallConnect.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpOutCallConnect::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_CONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> slave = getSlaveEndPoint(event);

	/* first CONNECT_ACK make the second call into CONNECT state */
	sendCallSignal(event, CALL_CONNECT_ACK);	/*send with Slave EndPoint */

	/* second CONNECT_ACK used to make 2 calls into 1 call */	
	Sptr <_CallConnectAck> connAck = new _CallConnectAck(); 
	connAck->setMasterEndPoint(getMasterEndPoint(event) );
	connAck->setSlaveEndPoint ( slave);

	CallEvent *callEvent = new CallEvent();
 	callEvent->setCallMsg( connAck );
	callEvent->setCallId( slave->getCallId() );

	/* CallInfo of master EP is in state of CONNECT,so it can be process differently */
	callEvent->setEndPoint( getMasterEndPoint(event) );
	sendSchedulerEvent( callEvent);

	return slave->findState( CONSOLE_EP_STATE_CONNECT );
}

