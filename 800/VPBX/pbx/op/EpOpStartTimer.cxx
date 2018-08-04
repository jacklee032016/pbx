/*
 * $Id: EpOpStartTimer.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "cpLog.h"

#include "EpOpBuilder.hxx"
#include "PbxEvent.hxx"
#include "PbxState.hxx"

#include "PbxConfiguration.hxx"

using namespace Assist;

const Sptr <PbxState>  EpOpStartTimer::process( const Sptr <PbxEvent> event )
{
	cpLog( LOG_DEBUG, "EpOpStartTimer operation" );

	if ( !setTimer( event, myDelay/*PbxConfiguration::instance()->getIsdnTimeout()*/ ) )
	{
		cpLog( LOG_DEBUG, "Pbx Timer not started" );
	}
	else
		cpLog( LOG_DEBUG, "Pbx Timer started" );

	return 0;
}

