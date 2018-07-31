/*
 * $Id: CallEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "CallEvent.hxx"
#include "CtrlDevice.hxx"
#include "AudioChannel.hxx"
#include "CallContainer.hxx"
#include "CommonMsg.hxx"

using namespace Assist;

CallEvent::CallEvent( )
	: PbxEvent( ),
	myCallId(0),
	myCallInfo(0)
//	,myCallContainer(0)
{
}

CallEvent::CallEvent(Sptr < Fifo < Sptr <PbxEvent> > > fifo)
	: PbxEvent( fifo),
	myCallId(0),
	myCallInfo(0)
// ,myCallContainer(0)
{
}

CallEvent::~CallEvent()
{
}

void CallEvent::setCallId(Sptr <CallId> _callId)
{
	myCallId = _callId;
}

Sptr <CallId> CallEvent::getCallId()
{
	return myCallId;
}

#if 0
void CallEvent::setCallInfo( const Sptr <CallInfo> callInfo, const Sptr <CallContainer> callContainer )
{
	assert( callInfo != 0 );
	assert( callContainer != 0 );

	myCallInfo = callInfo;
	myCallContainer = callContainer;
}
#endif

void CallEvent::setCallInfo( const Sptr <CallInfo> callInfo )
{
	if( callInfo == 0 )
	{
		cpLog(LOG_DEBUG, "Clearing CallInfo In CallEvent");
	}

	myCallInfo = callInfo;
	if( callInfo == 0 )
	{
		cpLog(LOG_DEBUG, "CallInfo In CallEvent has been cleared");
	}
}

Sptr <CallInfo> CallEvent::getCallInfo( ) const
{
	return ( myCallInfo );
}

void CallEvent::removeCallInfo()
{
	myCallInfo = 0;
//	myCallContainer = 0;
}

Sptr <CommonMsg>  CallEvent::getCallMsg()
{
	return myMsg;
}

void CallEvent::setCallMsg(Sptr <CommonMsg> _msg)
{
	myMsg = _msg;
}

