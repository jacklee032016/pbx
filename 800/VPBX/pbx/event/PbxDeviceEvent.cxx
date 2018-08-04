/*
 * $Id: PbxDeviceEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "PbxDeviceEvent.hxx"
#include "CallId.hxx"

using namespace Assist;

PbxDeviceEvent::PbxDeviceEvent (  )
	:PbxEvent()
{
}

PbxDeviceEvent::PbxDeviceEvent ( const Sptr < Fifo < Sptr <PbxEvent> > > outputFifo )
	:PbxEvent(outputFifo)
{
	assert( outputFifo != 0 );

	myFifo = outputFifo;
}

PbxDeviceEvent::~PbxDeviceEvent()
{
}

const char* const PbxDeviceEvent::name() const
{
	return "PBX Device Event";
}

