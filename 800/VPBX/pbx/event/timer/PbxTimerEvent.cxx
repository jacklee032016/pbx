/*
 * $Id: PbxTimerEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "PbxTimerEvent.hxx"

using namespace Assist;


PbxTimerEvent::PbxTimerEvent(Sptr < Fifo < Sptr <PbxEvent> > > destFifo)
    :   PbxEvent( destFifo)
{
	myId = 0;
}

PbxTimerEvent::~PbxTimerEvent()
{
	cpLog(LOG_DEBUG, "PbxTimerEvent destructing.....");
}

void PbxTimerEvent::cancelTimer()
{
	myFifo->cancel(myId);
//	myId = 0;
}

void PbxTimerEvent::startTimer(Sptr <PbxEvent> newEvent, const int timerMs)
{
	assert( newEvent != 0 );
	myId = myFifo->addDelayMs(newEvent, timerMs);
}

#if 0
void PbxTimerEvent::startTimer( const int timerMs)
{
	myId = myFifo->addDelayMs(this, timerMs);
}
#endif

FifoEventId PbxTimerEvent::getTimerId()
{
	return ( myId );
}

