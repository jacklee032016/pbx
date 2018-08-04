/* 
 * $Id: DigitTimeoutEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "DigitTimeoutEvent.hxx"

using namespace Assist;
DigitTimeoutEvent::DigitTimeoutEvent ()
{

}

DigitTimeoutEvent::DigitTimeoutEvent ( const Sptr < Fifo < Sptr <PbxEvent> > > outputFifo )
	: PbxEvent(outputFifo)
{
	assert( outputFifo != 0 );

	myFifo = outputFifo;

	/* added by lizhijie 2005.02.10 */
//	myDeviceId = AS_DEVICE_NOT_DEFINED;
}

const char* const DigitTimeoutEvent::name() const
{
	return "DigitTimeoutEvent";
}

DigitTimeoutEvent::~DigitTimeoutEvent()
{
}

