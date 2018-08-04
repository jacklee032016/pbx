/*
 * $Id: EpTimerEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "EpTimerEvent.hxx"

using namespace Assist;

EpTimerEvent::EpTimerEvent ( const Sptr < Fifo < Sptr <PbxEvent > > > destFifo )
	: PbxTimerEvent( destFifo )
{
}

EpTimerEvent::~EpTimerEvent()
{
}

const char* const EpTimerEvent::name() const
{
	return "EP Timer Event";
}

