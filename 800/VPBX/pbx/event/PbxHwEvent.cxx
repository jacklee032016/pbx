/*
 * $Id: PbxHwEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "PbxHwEvent.hxx"

using namespace Assist;

PbxHwEvent::PbxHwEvent()
{
	//signalOrRequest.request = 
	signalOrRequest.signal = DeviceSignalNULL;
    setPrivate(this);
}
PbxHwEvent::PbxHwEvent( const Sptr < Fifo < Sptr <PbxEvent> > > outputFifo )
	: PbxEvent(outputFifo)
{
	assert( outputFifo != 0 );
	myFifo = outputFifo;
    //TODO initialize data members: type and signalOrRequest
}

const char* const PbxHwEvent::name() const
{
	return "Hardware Event";
}

void PbxHwEvent::startTimer(Sptr <PbxEvent> newEvent, const int timerMs)
{
	assert( newEvent != 0 );
	myId = myFifo->addDelayMs(newEvent, timerMs);
}

PbxHwEvent::~PbxHwEvent()
{

}

