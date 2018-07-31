/*
* $Id: PstnOpStopRinging.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpStopRinging::process( const Sptr <PbxEvent> event )
{
#if 0
	/* callid is assigned when CallOpRouting  */
	Sptr <CallInfo> call = event->getEndPoint()->getCallInfo();
	assert(call != 0);
	Sptr <CallId> cid = call->getCallId();
	assert( cid != 0 );
	event->getEndPoint()->setCallId(call->getCallId());
#endif

	if(event->getEndPoint()->checkExt() )
	{/* only stop ring on FXS when leave RINGING state */
		sendCommand(event, DeviceSignalRingStop);
	}
	
	return PBX_STATE_CONTINUE;
}

