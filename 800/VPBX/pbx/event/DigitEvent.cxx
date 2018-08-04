/*
 * $Id: DigitEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "DigitEvent.hxx"

using namespace Assist;

DigitEvent::DigitEvent()
	: PbxEvent( )
{
}

DigitEvent::DigitEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo)
	: PbxEvent( fifo)
{
}

DigitEvent::~DigitEvent()
{
}

