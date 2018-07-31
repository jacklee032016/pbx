/*
* $Id: PstnOpQueryStatus.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpQueryStatus::process( const Sptr <PbxEvent> event )
{

	sendCommand(event, DeviceSignalQueryStatus );
	
	return PBX_STATE_CONTINUE;
}

