/*
* $Id: VoipOpFwdDigitInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

/* rx DTMF digit encoded in SETUP msg when in the state of CALL*/
const Sptr <PbxState> VoipOpFwdDigitInCall::process( const Sptr<PbxEvent> event )
{
	if( checkCallMsg( event) != CALL_SETUP )
	{
		return PBX_STATE_CONTINUE;
	}
	
	Sptr <_CallSetup> setupMsg = NULL;
	setupMsg.dynamicCast(isCallMsg( event) );
	if(setupMsg==0) /* not the msg handled in this operator */
	{	
		cpLog(LOG_ERR, "No SETUP CallEvent in this callEvent");
		return PBX_STATE_CONTINUE;
	}

	cpLog(LOG_ERR, "VOIP forward digit in RTP stack");

	const char *digit = setupMsg->getTo();
	if(digit!=0)
	{
		Sptr <VoipAudioChannel> audio;
		Sptr <EndPoint> ep = event->getEndPoint();
		assert(ep!=0);
		audio.dynamicCast( ep->getAudio() );
		assert(audio!=0);
		
		audio->transmitRTPDTMF(digit[0]);
		
	}
	return PBX_STATE_CONTINUE;
}

