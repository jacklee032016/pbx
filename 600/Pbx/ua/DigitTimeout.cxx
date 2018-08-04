/*
 * $Id: DigitTimeout.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "DigitTimeout.hxx"
#include "UaDevice.hxx"
#include "UaDigitTimerEvent.hxx"

using namespace Vocal;

DigitTimeout* DigitTimeout::uaTimeout = 0;
	
DigitTimeout* DigitTimeout::instance()
{
	assert( uaTimeout != 0 );
	return uaTimeout ;
}


void DigitTimeout::instance(Sptr < Fifo < Sptr < SipProxyEvent > > > cpq,
                      int initTimeout , int interDigitTimeout )
{
	assert( uaTimeout == 0 );
	uaTimeout = new DigitTimeout(cpq, initTimeout, interDigitTimeout);
	assert( uaTimeout != 0 );
}

DigitTimeout::DigitTimeout( Sptr < Fifo < Sptr < SipProxyEvent > > > cpq,
                            int initTimeout,
                            int interDigitTimeout )
{
	initTimeoutVal = initTimeout;
	interDigitTimeoutVal = interDigitTimeout;
#if 0
	isTimeout = false;
	id = 0;
#endif
	callProcessingQueue = cpq;

	//spawing the timeout thread
	digitTimeoutThread.spawn(digitTimeoutThreadWrapper, this);
	cpDebug(LOG_DEBUG, "Spawning the interdigit timeout thread");
}

///
void DigitTimeout::timerReset(Sptr <PendingDigitTimerEvent > pendingEvent)
{
	digitTimeoutMutex.lock();
	pendingEvent->isTimeout = false;
	digitTimeoutFifo.cancel( pendingEvent->eventId );
	pendingEvent->eventId = digitTimeoutFifo.addDelayMs( pendingEvent, initTimeoutVal);
	digitTimeoutMutex.unlock();
}

///
void DigitTimeout::timerStart(Sptr <PendingDigitTimerEvent > pendingEvent)
{
	digitTimeoutMutex.lock();
	digitTimeoutFifo.cancel( pendingEvent->eventId );
	//    pendingEvent->eventId  = digitTimeoutFifo.addDelayMs(INTER_DIGIT_TIME_OUT, interDigitTimeoutVal);
	pendingEvent->eventId  = digitTimeoutFifo.addDelayMs(pendingEvent, interDigitTimeoutVal);
	digitTimeoutMutex.unlock();
}

void DigitTimeout::timerCancel(Sptr <PendingDigitTimerEvent > pendingEvent)
{
	digitTimeoutFifo.cancel(pendingEvent->eventId );

	digitTimeoutMutex.lock();
	pendingEvent->eventId  = 0;
	pendingEvent->isTimeout = false;
	digitTimeoutMutex.unlock();
}

///
void* DigitTimeout::digitTimeoutThreadWrapper(void* timeout)
{
	assert(timeout);

	DigitTimeout* dTimeout = static_cast < DigitTimeout* > (timeout);
	dTimeout->digitTimeoutMain();
	return 0;
}

///
void DigitTimeout::digitTimeoutMain()
{
	cpLogSetLabelThread (VThread::selfId(), "DigitTimer");
	
	cpDebug(LOG_DEBUG, "PID of DigitTimeoutThread is %d" ,getpid() );
	
	cpDebug(LOG_DEBUG, "starting digitTimeout" );
	while ( true )
	{
		Sptr <PendingDigitTimerEvent > pending = digitTimeoutFifo.getNext();

		if ( pending->type == INTER_DIGIT_TIME_OUT )
		{
			digitTimeoutMutex.lock();
			pending->isTimeout = true;
			digitTimeoutMutex.unlock();

			Sptr < UaDigitTimerEvent > timerEvent = new UaDigitTimerEvent( callProcessingQueue );
			assert( timerEvent != 0 );
			timerEvent->callId = UaDevice::instance()->getCallId( pending->deviceId );
			timerEvent->setDeviceId( pending->deviceId );
			callProcessingQueue->add( timerEvent );
		}
	}
}

