/*
* $Id: PbxStateObject.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "PbxStateObject.hxx"
#include "PbxManager.hxx"
#include "PbxStateMachine.hxx"
#include "CtrlDevice.hxx"

#include "PbxTimerEvent.hxx"

using namespace Assist;

PbxStateObject::PbxStateObject(Sptr <PbxManager> _parent )
	:ManagedObject( _parent)
{
	myStateMachine = getParent()->getPbxStateMachine();
	if(myStateMachine == 0)
	{
		cpLog(LOG_DEBUG, "%s has no statemachine when create new StateObject", getParent()->getName() );
		assert(0);
	}
}

PbxStateObject::~PbxStateObject(void)
{
	cpLog(LOG_DEBUG, "Destorying.....");
}

Sptr <PbxState> PbxStateObject::findState(int stateType)
{
	return myStateMachine->findState(stateType);
}

void PbxStateObject::setState(Sptr <PbxState> _state)
{
	myState = _state;
}

void PbxStateObject::process( Sptr <PbxEvent> msg)
{
	myStateMachine->process( msg);

	return ;
}

void PbxStateObject::lock()
{
	cpLog( LOG_DEBUG, "Locking CallInfo" );
	myMutex.lock();
}

void PbxStateObject::unlock()
{
	cpLog( LOG_DEBUG, "UnLocking CallInfo" );
	myMutex.unlock();
}

void  PbxStateObject::setTimer( const Sptr <PbxTimerEvent> timerEvent )
{
	assert ( timerEvent != 0 );  
	myTimer = timerEvent;
}

Sptr <PbxTimerEvent> PbxStateObject::getTimer() const
{
	return ( myTimer );
}

void PbxStateObject::removeTimer()
{
	myTimer = 0;
}

Sptr <PbxStateMachine> PbxStateObject::getStateMachine()
{
	return myStateMachine;
}

Sptr <PbxState> PbxStateObject::getState()
{
	return myState;
}

