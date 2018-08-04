/*
 * $Id: PstnOpAnswerCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpAnswerCall::process( const Sptr <PbxEvent> event )
{
	DeviceEventType eventType = getPstnEventType(event);
	
	if ( eventType != DeviceEventHookUp && eventType != DeviceEventFlash )
	{
		return PBX_STATE_CONTINUE;
	}
	
	cpLog( LOG_DEBUG, "Answer Call" );

	Sptr <EndPoint> pep = event->getEndPoint();
	assert(pep != 0);

	Sptr <AudioChannel> audio = pep->getAudio();
	assert(audio!=0);
	audio->activate();
#if 0
	/* DT detect and dial out is in the same thread */
	if (!pep->checkExt() )
	{/* send out DTMF Called phone number to the external switch */
		sendCommand(event, DeviceSignalFxoDialing);
	}
#endif

	sendCallSignal(event, CALL_CONNECT);

	return lookupEPState(event, PSTN_EP_STATE_INCALL);
}

