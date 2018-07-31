/*
 * $Id: CallTimerEvent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "CallTimerEvent.hxx"
#include "CallInfo.hxx"
#include "CallContainer.hxx"

using namespace Assist;

CallTimerEvent::CallTimerEvent ( const Sptr < Fifo < Sptr <PbxEvent > > > destFifo )
	: PbxTimerEvent( destFifo )
{
}

CallTimerEvent::~CallTimerEvent()
{
}

const char* const CallTimerEvent::name() const
{
	return "Call Timer Event";
}

void CallTimerEvent::setCallId( const Sptr <CallId> callid )
{
	callId = callid;
}

const Sptr < CallId > CallTimerEvent::getCallId() const
{
	return callId;
}

void CallTimerEvent::removeCall()
{
/*
	assert( myCallInfo != 0 );
	assert( myCallContainer != 0 );

	Sptr <CallInfo> callInfo = getCallInfo();
	assert(callInfo!=0);
	Sptr <CallId> callid = callInfo->getCallId();
	assert( callid != 0 );
	
	myCallContainer->removeCall(callid);
*/	
}

