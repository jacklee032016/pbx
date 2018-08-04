/*
 * $Id: CallContainer.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "global.h"
#include "CallContainer.hxx"
#include "LockHelper.hxx"
#include "CallInfo.hxx"

using namespace Assist;

CallContainer::CallContainer()
{
}

CallContainer::~CallContainer()
{
	cpLog(LOG_DEBUG, "Destorying.....");
//	LockHelper lockHelper(myMutex);
	myCallInfos.clear();
}

Sptr <CallInfo> CallContainer::getCall(const Sptr <CallId> newOrExistingCallId)
{
	assert ( newOrExistingCallId != 0 );
	Sptr <CallInfo> newCall = 0;

	LockHelper lockHelper(myMutex);

	TableIter iter = myCallInfos.find(newOrExistingCallId);

	if ( iter != myCallInfos.end() )
	{
		cpLog(LOG_DEBUG, "getCall(): CallLeg (%s) already exists. ",newOrExistingCallId->encode().logData());
		newCall = iter->second;
	}
	else
	{
		newCall = new CallInfo;
		newCall->setCallId(newOrExistingCallId);
		cpLog(LOG_DEBUG, "getCall(): add a new entry in CallContainer\n%s",  newOrExistingCallId->encode().logData());
		myCallInfos[newOrExistingCallId] = newCall;
	}

	assert ( newCall != 0 );

	return ( newCall );
}


Sptr <CallInfo> CallContainer::findCall(const Sptr <CallId> newOrExistingCallId)
{
	assert ( newOrExistingCallId != 0 );
	Sptr < CallInfo > newCall = 0;

	LockHelper lockHelper(myMutex);

	cpLog(LOG_DEBUG, "To find an entry in CallContainer\n%s",  newOrExistingCallId->encode().logData());

	cpLog( LOG_DEBUG, "there are %d entries", myCallInfos.size() );
	TableIter iter = myCallInfos.find( newOrExistingCallId);

	if ( iter != myCallInfos.end() )
	{
		cpLog( LOG_DEBUG, "found" );
		newCall = iter->second;
	}
	else
	{
		cpLog( LOG_DEBUG, "not found" );
	}

	return ( newCall );
}


Sptr <CallInfo> CallContainer::addCall(const Sptr <CallId> _callId, const Sptr <CallInfo> _callInfo)
{
	assert ( _callId != 0 );
	assert ( _callInfo != 0 );
	LockHelper lockHelper(myMutex);

	cpLog(LOG_DEBUG, "addCall(): add a new entry in CallContainer\n%s", _callId->encode().logData());
	myCallInfos[_callId] = _callInfo;

	return ( _callInfo );
}

void CallContainer::removeCall(const Sptr <CallId > _callId)
{
	assert ( _callId != 0 );
	LockHelper lockHelper(myMutex);

	TableIter iter = myCallInfos.find( _callId );
	cpLog(LOG_DEBUG, "remove entry in CallContainer\n%s", _callId->encode().logData());

	if ( iter != myCallInfos.end() )
	{
		iter->second->removeTimer();
		myCallInfos.erase(iter);
	}
}

