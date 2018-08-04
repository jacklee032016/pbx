/*
 * $Id: ConsoleOpStopDialTone.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> ConsoleOpStopDialTone::process( const Sptr < PbxEvent > event )
{
	Sptr <DigitEvent> digitEvent = isDigit(event);
	if ( digitEvent == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	if ( digitEvent->myDigit != ' ' )
	{
		cpLog( LOG_DEBUG, "Stop Dial Tone" );
		Sptr <ConsoleEndPoint> cep;
		cep.dynamicCast(event->getEndPoint() );
		assert( cep != 0 );
		cep->stopDialTone();
	}

	return PBX_STATE_CONTINUE;
}

