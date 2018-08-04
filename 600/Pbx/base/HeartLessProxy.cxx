/*
 * $Id: HeartLessProxy.cxx,v 1.3 2007/06/18 11:11:17 lizhijie Exp $
 */

#include "global.h"
#include "HeartLessProxy.hxx"
#include "SipTransceiverFilter.hxx"
#include "Builder.hxx"
#include "SipThread.hxx"
#include "WorkerThread.hxx"

using namespace Vocal;

HeartLessProxy::HeartLessProxy( 
	const Sptr < Builder >  builder,
	unsigned short          defaultSipPort,
	Data                    applName,
	bool                    filterOn, 
	bool                    nat,
	SipAppContext           aContext
)
{
	myCallContainer = new CallContainer;

	myBuilder = builder;
	myBuilder->setCallContainer(myCallContainer);

	myCallProcessingQueue = new Fifo < Sptr < SipProxyEvent > >;

	myWorkerThread = new WorkerThread(myCallProcessingQueue, myBuilder);

	//  Filter option controls which transceiver object is created for the
	//  sip stack.
	//
	//  If filterOn is set, they get a SipTransceiverFilter, otherwise they
	//  get a vanilla SipTransceiver.
	//
	if ( filterOn == true )
	{
		mySipStack = new SipTransceiverFilter(applName, defaultSipPort, nat, aContext);
	}
	else
	{
		mySipStack = new SipTransceiver(applName, defaultSipPort, nat, aContext);
	}

	myBuilder->setSipStack(mySipStack);

	mySipThread = new SipThread(mySipStack, myCallProcessingQueue);    

	assert( myCallContainer != 0 );
	assert( myWorkerThread != 0 );
	assert( myCallProcessingQueue != 0 );
	assert( myBuilder != 0 );
	assert( mySipStack != 0 );
}


HeartLessProxy::~HeartLessProxy()
{
}


void HeartLessProxy::run()
{
	myWorkerThread->run();
	mySipThread->run();
}

void HeartLessProxy::shutdown()
{
	myWorkerThread->shutdown();
	mySipThread->shutdown();
}

void HeartLessProxy::join()
{
	myWorkerThread->join();
	mySipThread->join();
}

