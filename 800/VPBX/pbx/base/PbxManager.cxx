/*
 * $Id: PbxManager.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include "PbxManager.hxx"
#include "PbxStateMachine.hxx"
#include "GatewayMgr.hxx"
#include "DeviceMgr.hxx"
#include "PbxTimerEvent.hxx"
#include "PbxAgent.hxx"

using namespace Assist;

PbxManager::PbxManager(PbxAgent *agent, const char *_name)
	:myMutex(),
	myAgent(agent)
{
	cpLog( LOG_DEBUG, "PbxManager" );
	sprintf(myName, "%s", _name);
	myQueue = new Fifo < Sptr <PbxEvent> >;
	assert( myQueue != 0 );
}


PbxManager::~PbxManager()
{
	cpLog(LOG_DEBUG, "Destorying.....");
}

Sptr <PbxTimerEvent> PbxManager::createTimerEvent()
{
	return new PbxTimerEvent( myQueue);
}

void  PbxManager::mainLoop (void* params)
{
	myMainLoop(params);

//	if( myQueue->size() > 0 )
	{
//		myMutex.lock();
		cpLog(LOG_DEBUG, "%s waiting event on 0x%x.....", getName(), &myQueue );
		Sptr <PbxEvent> event =  myQueue->getNext();
		cpLog( LOG_DEBUG, "Signal from other thread" );
//		myMutex.unlock();
		
		processEvent(event);
//		cpLog(LOG_DEBUG, "%s end of event process, waiting event on 0x%x.....", getName(), &myQueue );
	}
}

void PbxManager::processEvent( Sptr<PbxEvent> event )
{
//	TRACE;
//	cpLog( LOG_DEBUG, "process Event %s", event->name() );

	if( event == 0)
		return;
	
	myProcessEvent( event);
}

void PbxManager::execute()
{
	if( startup() )
		assert(0);
	myThread = new PbxManagerThread(this );

	myThread->run();
}

void PbxManager::join()
{
	assert(myThread != 0 );

	myThread->join();
}

void PbxManager::shutdown()
{
	assert( myQueue != 0 );
	myQueue->add(0);

	assert(myThread != 0 );
	myThread->shutdown();
}

Sptr <PbxStateMachine>  PbxManager::getPbxStateMachine()
{
	return myPbxStateMachine;
}

void PbxManager::setPbxStateMachine(PbxStateMachine *stateMachine)
{
	cpLog( LOG_DEBUG, " set PbxStateMachine" );
	myPbxStateMachine = stateMachine;
}

const char *PbxManager::getName() const
{
	return  myName;
}

Sptr < Fifo < Sptr <PbxEvent> > > PbxManager::getQueue()
{
	return myQueue;
}

void PbxManager::reportEvent( Sptr <PbxEvent> event)
{
	cpLog(LOG_DEBUG, "Add event to 0x%x", &myQueue);
	myQueue->add(event);
}

