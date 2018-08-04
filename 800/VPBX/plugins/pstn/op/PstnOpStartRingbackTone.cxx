/*
 * $Id: PstnOpStartRingbackTone.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpStartRingbackTone::process( const Sptr <PbxEvent> event )
{
	call_method_t method = checkCallMsg(event);
	if( method != CALL_ALERTING && method != CALL_PROCEEDING )
	{
		return PBX_STATE_CONTINUE;
	}

	if(method==CALL_ALERTING)
		sendCommand(event,  DeviceSignalLocalRingbackStart);
	
	return PBX_STATE_DONE_WITH_EVENT;
}

