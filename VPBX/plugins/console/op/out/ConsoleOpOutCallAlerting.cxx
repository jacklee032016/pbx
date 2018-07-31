/*
 * $Id: ConsoleOpOutCallAlerting.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpOutCallAlerting::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_ALERTING )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <EndPoint> slave = getSlaveEndPoint(event);

	/* cancel timer for CONSOLE_OUT_CALLING */
	cancelTimer(event);
	
	/* notify Master EndPoint */
#if 0
	/* forward ALERTING_CALL to master */
	callEvent->setCallId( ctrl->getMasterEndPoint()->getCallId() );
	callEvent->setEndPoint( ctrl->getMasterEndPoint() );
	PbxAgent::instance()->getGateway()->reportEvent( callEvent);
#else
	/* modify the audioChannel audiable voice directly */
	setAudioChannel(event, console_audio_ringback);
#endif

	/* start a new timer for OUT_ALERTING */
	setTimer(event, PbxConfiguration::instance()->getConsoleAlertingTimeout() );

	return slave->findState( CONSOLE_EP_STATE_OUT_ALERTING );
}

