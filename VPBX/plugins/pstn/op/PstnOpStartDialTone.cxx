/*
 * $Id: PstnOpStartDialTone.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpStartDialTone::process( Sptr <PbxEvent> event )
{
	Sptr <EndPoint> ep = event->getEndPoint();
	assert( ep != 0 );
	ep->resetDialing();

	sendCommand(event, DeviceSignalDialToneStart);

	return PBX_STATE_CONTINUE;
}

