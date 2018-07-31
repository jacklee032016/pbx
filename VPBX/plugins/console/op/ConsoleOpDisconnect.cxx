/*
 * $Id: ConsoleOpDisconnect.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpDisconnect::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_DISCONNECT )
	{
		return PBX_STATE_CONTINUE;
	}


	/* _CallDisConnect can be rx both by master and slave */
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep!=0);

//	setAudioChannel(event, console_audio_silence);

	Sptr <AudioChannel> audio = ep->getAudio();
	assert(audio!=0);
	audio->deactivate();
	
	if(ep->getState()->getStateType() == CONSOLE_EP_STATE_CONNECT
		|| (ep->getState()->getStateType() == CONSOLE_EP_STATE_BUILDECALL) )
	{
	
		Sptr <EndPoint> master = getMasterEndPoint(event);
		Sptr <EndPoint> slave = getSlaveEndPoint(event);

		Sptr <EndPoint> peer = master;
		if(master == ep )
		{
			peer = slave;
		}
		
		if( peer->getCallId() != 0)
		{/* forward this Disconnect to the second call's endpoint */
			event->setEndPoint( peer);
			Sptr <CallEvent> callEvent = isCallEvent(event);
			assert(callEvent != 0);
			callEvent->setCallId( peer->getCallId());

			sendSchedulerEvent(event);
		}

		peer->reset();
		peer->setState( peer->findState(CONSOLE_EP_STATE_IDLE));
		
	}

	ep->reset();
	ep->setState(ep->findState( CONSOLE_EP_STATE_IDLE ) );
	
	return ep->findState( CONSOLE_EP_STATE_IDLE );
}

