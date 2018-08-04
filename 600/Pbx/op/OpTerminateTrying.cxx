/*
 * $Id: OpTerminateTrying.cxx,v 1.3 2007/03/07 18:24:08 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include <cpLog.h>

#include "CancelMsg.hxx"
#include "OpTerminateTrying.hxx"
#include "UaDeviceEvent.hxx"
#include "UaCallInfo.hxx"
#include "TimerEvent.hxx"

using namespace Vocal;

OpTerminateTrying::OpTerminateTrying()
{}

const Sptr < State > OpTerminateTrying::process( Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "TerminateTrying operation" );

	Sptr < SipTransceiver > stack;

	if(!isTimeout(event))
	{
		Sptr < UaDeviceEvent > deviceEvent;
		deviceEvent.dynamicCast( event );
		if ( deviceEvent == 0 )
		{
			return 0;
		}
		 
		if ( deviceEvent->type != DeviceEventHookDown )
		{
			return 0;		
		}

		stack = deviceEvent->getSipStack();
	}
	else	
	{   
		Sptr < TimerEvent > timerEvent;
		timerEvent.dynamicCast( event );
		if ( timerEvent == 0 )
		{
			return 0;
		}
		stack = timerEvent->getSipStack();	
	}
	
	Sptr < UaCallInfo > call;
	call.dynamicCast( event->getCallInfo() );
	assert( call != 0 );

	Sptr < Contact > contact;
	Sptr < ContactList > contacts = call->getContactList();
	ContactList::iterator iter = contacts->begin();
	
	while ( iter != contacts->end() )
	{
		contact = (*iter);
		assert( contact != 0 );
		if ( contact->getStatus() < 200 )
		{
			cpDebug(LOG_DEBUG, "CANCEL contact" );
			CancelMsg msg( contact->getInviteMsg() );
			stack->sendAsync( msg );
		}
		
		iter++;
	}

	return 0;
}

OpTerminateTrying::~OpTerminateTrying()
{}

const char* const OpTerminateTrying::name() const
{
	return "OpTerminateTrying";
}

