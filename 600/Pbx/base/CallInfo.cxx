/*
 * $Id: CallInfo.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "CallInfo.hxx"
#include "TimerEvent.hxx"


using namespace Vocal;


CallInfo::CallInfo()
    :   myFeature(0),
        myState(0),
        mySipCommand(0),
        myTimer(0),
        myCallData(0)
{ 
}


CallInfo::CallInfo(const CallInfo & rhs)
{
      copyRhsToThis(rhs); 
}


const CallInfo &
CallInfo::operator=(const CallInfo & rhs)
{
    if ( this != &rhs )
    {
        copyRhsToThis(rhs);
    }

    return ( *this );
}


CallInfo::~CallInfo()
{
	 
}


void
CallInfo::setFeature(const Sptr < Feature > nextFeature)
{
    myFeature = nextFeature;
}



Sptr < Feature >
CallInfo::getFeature() const
{
    return ( myFeature );
}



void
CallInfo::setState( const Sptr < State > nextState )
{
    myState = nextState;
}


Sptr < State >
CallInfo::getState() const
{
    return ( myState );
}


void
CallInfo::setSipCommand( const Sptr < SipCommand > sipCommand )
{
    mySipCommand = sipCommand;
}


Sptr < SipCommand >
CallInfo::getSipCommand() const
{
    return ( mySipCommand );
}


void
CallInfo::setTimer( const Sptr < TimerEvent > timerEvent )
{
    assert ( timerEvent != 0 );

    myTimer = timerEvent;
}



Sptr < TimerEvent >
CallInfo::getTimer() const
{
    return ( myTimer );
}



void
CallInfo::removeTimer()
{
    myTimer = 0;
}



Sptr < SipCallLeg >
CallInfo::getSipCallLeg() const
{
    Sptr < SipCallLeg > retSipCallLeg = new SipCallLeg;

    assert( mySipCommand != 0 );

    *retSipCallLeg = mySipCommand->computeCallLeg();

    return ( retSipCallLeg );
}



void
CallInfo::setCallStateData(Sptr < StateMachnData > callData)
{
    myCallData = callData;
}



Sptr < StateMachnData >
CallInfo::getCallStateData() const
{
    return ( myCallData );
}



void
CallInfo::lock()
{
    cpDebug(LOG_DEBUG, "Locking CallInfo" );
    myMutex.lock();
}



void
CallInfo::unlock()
{
    cpDebug(LOG_DEBUG, "UnLocking CallInfo" );
    myMutex.unlock();
}


void
CallInfo::copyRhsToThis(const CallInfo & rhs)
{
    myFeature = rhs.myFeature;
    myState = rhs.myState;
    mySipCommand = rhs.mySipCommand;
    myTimer = rhs.myTimer;
    myCallData = rhs.myCallData;
}

