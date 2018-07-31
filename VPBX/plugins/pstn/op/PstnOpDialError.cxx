/*
 * $Id: PstnOpDialError.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"
#include "DigitTimeoutEvent.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpDialError::process( const Sptr <PbxEvent> event )
{
	Sptr <DigitTimeoutEvent> timerEvent;
	timerEvent.dynamicCast( event );
	if ( timerEvent == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	return lookupEPState(event, PSTN_EP_STATE_ERROR);
}

