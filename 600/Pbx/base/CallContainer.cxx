/*
 * $Id: CallContainer.cxx,v 1.3 2007/05/06 09:48:54 lizhijie Exp $
 */

#include "global.h"
#include "CallContainer.hxx"
#include "LockHelper.hxx"


using namespace Vocal;

CallContainer::CallContainer()
{
}



CallContainer::~CallContainer()
{
    LockHelper lockHelper(myMutex);

    myCallInfo.clear();
}



Sptr < CallInfo >
CallContainer::getCall(const Sptr < SipCallLeg > newOrExistingCallLeg)
{
    assert ( newOrExistingCallLeg != 0 );

    Sptr < CallInfo > newCall = 0;

    LockHelper lockHelper(myMutex);

    TableIter iter = myCallInfo.find(newOrExistingCallLeg);
    
    if ( iter != myCallInfo.end() )
    {
	cpDebug(LOG_DEBUG, "getCall(): CallLeg (%s) already exists. ",newOrExistingCallLeg->encode().logData());
        newCall = iter->second;
    }
    else
    {
	newCall = new CallInfo;
	
	cpDebug(LOG_DEBUG, "getCall(): add a new entry in CallContainer\n%s",
	      newOrExistingCallLeg->encode().logData());
	
	myCallInfo[newOrExistingCallLeg] = newCall;
    }

    assert ( newCall != 0 );

    return ( newCall );
}


Sptr < CallInfo >
CallContainer::findCall(const Sptr < SipCallLeg > newOrExistingCallLeg)
{
    assert ( newOrExistingCallLeg != 0 );

    Sptr < CallInfo > newCall = 0;

    LockHelper lockHelper(myMutex);

    cpDebug(LOG_DEBUG, "To find an entry in CallContainer\n%s",
          newOrExistingCallLeg->encode().logData());

    cpDebug(LOG_DEBUG, "there are %d entries", myCallInfo.size() );

    TableIter iter = myCallInfo.find(newOrExistingCallLeg);
    
    if ( iter != myCallInfo.end() )
    {
	cpDebug(LOG_DEBUG, "found" );
	newCall = iter->second;
    }
    else
    {
	cpDebug(LOG_DEBUG, "not found" );
    }

    return ( newCall );
}


Sptr < CallInfo >
CallContainer::addCall(const Sptr < SipCallLeg > newOrExistingCallLeg,
                       const Sptr < CallInfo > theCallInfo)
{
    assert ( newOrExistingCallLeg != 0 );
    assert ( theCallInfo != 0 );

    LockHelper lockHelper(myMutex);

    cpDebug(LOG_DEBUG, "addCall(): add a new entry in CallContainer\n%s",
          newOrExistingCallLeg->encode().logData());

    myCallInfo[newOrExistingCallLeg] = theCallInfo;

    return ( theCallInfo );
}



void
CallContainer::removeCall(const Sptr <SipCallLeg> existingCall)
{
    assert ( existingCall != 0 );

    LockHelper lockHelper(myMutex);

    TableIter iter = myCallInfo.find( existingCall );

    cpDebug(LOG_DEBUG, "remove entry in CallContainer\n%s",
          existingCall->encode().logData());

    if ( iter != myCallInfo.end() )
    {
        myCallInfo.erase(iter);
    }
}
