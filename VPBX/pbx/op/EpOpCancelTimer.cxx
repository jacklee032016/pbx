/*
* $Id: EpOpCancelTimer.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */


#include "EpOpBuilder.hxx"

#include "PbxState.hxx"
#include "PbxEvent.hxx"

using namespace Assist;

const Sptr <PbxState> EpOpCancelTimer::process( const Sptr <PbxEvent> event )
{
	cpLog( LOG_DEBUG, "EpOpCancelTimer operation" );

	cancelTimer(event);
#if 0	
	Sptr <EndPoint> ep = event->getEndPoint();
	assert(ep != 0);
	ep->reset();
#endif

	return 0;
}

