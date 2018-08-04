/*
 * $Id: PstnOpStopDialTone.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnOpBuilder.hxx"
#include "DigitTimeoutEvent.hxx"

using namespace Assist;

const Sptr <PbxState> PstnOpStopDialTone::process( const Sptr <PbxEvent> event )
{

#if 1
	Sptr <DigitEvent> digitEvent = isDigit(event);
	if ( digitEvent == 0 )
#else
	Sptr < PstnEvent > deviceEvent;
	deviceEvent.dynamicCast( event );
	if ( deviceEvent == 0 )
#endif		
	{
		Sptr <DigitTimeoutEvent> timerEvent;
		timerEvent.dynamicCast( event );
		if ( timerEvent == 0 )
		{
			return PBX_STATE_CONTINUE;
		}
	}

	cpLog( LOG_DEBUG, "Stop Dial Tone" );
   
	sendCommand(event, DeviceSignalDialToneStop);

	return PBX_STATE_CONTINUE;
}

