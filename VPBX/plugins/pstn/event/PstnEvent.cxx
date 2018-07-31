/*
 * $Id: PstnEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "PstnEvent.hxx"

using namespace Assist;

PstnEvent::PstnEvent()
	: PbxEvent( ),
	  type(DeviceEventUndefined)
{
}

PstnEvent::PstnEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo)
	: PbxEvent( fifo)
{
}

PstnEvent::~PstnEvent()
{
}


