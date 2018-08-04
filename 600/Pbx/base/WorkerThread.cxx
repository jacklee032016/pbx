/*
 * $Id: WorkerThread.cxx,v 1.3 2007/03/28 19:03:39 lizhijie Exp $
 */

#include "global.h"
#include "WorkerThread.hxx"

#include "UaDevice.hxx"
#include "UaHardwareEvent.hxx"

using namespace Vocal;


WorkerThread::WorkerThread(const Sptr < Fifo < Sptr <SipProxyEvent > > > inputFifo,
    const Sptr <Builder> builder )
    : 	myFifo(inputFifo), 
    	myBuilder(builder)
{
	assert(myFifo != 0 );
	assert(myBuilder != 0 );
}

WorkerThread::~WorkerThread()
{
}


void WorkerThread::shutdown()
{
	assert( myFifo != 0 );

	ThreadIf::shutdown();

	myFifo->add(0);
}


void WorkerThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), "Worker");
	cpDebug(LOG_DEBUG, "PID of WorkerThread is %d" ,getpid() );
	
	while ( true )
	{
		assert( myFifo != 0 );
		assert( myBuilder != 0 );

		Sptr < SipProxyEvent > nextEvent = myFifo->getNext();

		if ( isShutdown() == true )
		{
			cpDebug(LOG_DEBUG, "Shutting down WorkerThread");
			return;
		}

		assert( nextEvent != 0 );
		if (nextEvent == 0)
			continue;

		cpDebug(LOG_DEBUG, "WorkerThread::thread() processing event");


		Sptr<UaHardwareEvent> hardwareEvent;
		hardwareEvent.dynamicCast( nextEvent );
		if(hardwareEvent != 0)
		{
			cpDebug(LOG_DEBUG, "WorkerThread::thread() processing HardwareEvent event for PBX");
			UaDevice::getDeviceQueue()->add( hardwareEvent );
			continue;
		}


		try
		{
			myBuilder->process(nextEvent);
		}
		catch ( VException& v )
		{
			///TODO: memory release (UaCallInfo)
			cpDebug(LOG_DEBUG, "VException while processing %s", v.getDescription().c_str() );
		}
		catch ( ... )
		{
			///TODO: memory release (UaCallInfo)
			cpDebug(LOG_DEBUG, "Exception while processing" );
		}

		cpDebug(LOG_DEBUG, "WorkerThread::thread() done processing event");
	}
}

