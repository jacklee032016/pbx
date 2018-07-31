/*
 * $Id: ConsoleOpOutCallConnectAck.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpOutCallConnectAck::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_CONNECT_ACK )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> slave = getSlaveEndPoint(event);

	Sptr <EndPoint> master = getMasterEndPoint(event) ;

	slave->reset();
	slave->setState( slave->findState( CONSOLE_EP_STATE_IDLE) );

	master->reset();
	master->setState( master->findState( CONSOLE_EP_STATE_IDLE) );
	
	return master->findState( CONSOLE_EP_STATE_IDLE );
}

