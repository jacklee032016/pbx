/*
 * $Id: PstnOpStartErrorTone.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpStartErrorTone::process( Sptr <PbxEvent> event )
{
	sendCommand(event, DeviceSignalFastBusyStart );

	return PBX_STATE_CONTINUE;
}

