/*
 * $Id: OpFarEndBusy.cxx,v 1.6 2007/06/18 11:11:17 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>

#include "OpFarEndBusy.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"

using namespace Vocal;

OpFarEndBusy::OpFarEndBusy()
{}

OpFarEndBusy::~OpFarEndBusy()
{}

const char* const OpFarEndBusy::name() const
{
	return "OpFarEndBusy";
}

const Sptr <State> OpFarEndBusy::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpFarEndBusy operation" );

	Sptr <SipEvent> sipEvent;
	sipEvent.dynamicCast( event );
	if ( sipEvent == 0 )
	{
		return 0;
	}
	
	Sptr <SipMsg> sipMsg = sipEvent->getSipMsg();
	assert( sipMsg != 0 );

	Sptr <StatusMsg> msg;
	msg.dynamicCast( sipMsg );
	if ( msg == 0 )
	{
		return 0;
	}

	Sptr <UaCallInfo> call;
	call.dynamicCast( event->getCallInfo() );
	assert( call != 0 );

	int status;
	int best = 1000;
	bool haveBusy = false;

	Sptr <Contact> contact;
	Sptr <ContactList> contacts = call->getContactList();
	ContactList::iterator iter = contacts->begin();
	while ( iter != contacts->end() )
	{
		contact = (*iter);
		assert( contact != 0 );
		status = contact->getStatus();

		if ( status < best )
		{
			best = status;
			cpDebug(LOG_DEBUG, "best == %d", best );
		}
		
		if ( status == 486 )
		{
			haveBusy = true;
			cpDebug(LOG_DEBUG, "status == %d(%s) : IP %s", status , 
				contact->getStatusMsg().encode().logData(),  contact->getStatusMsg().getReceivedIPName().logData() );
		}
		iter++;
	}
	
	if ( best >= 300 && haveBusy )
	{
		UaDevice::instance()->releaseRtpPort( event->getDeviceId() );

		cpDebug(LOG_DEBUG, "condition: if( best >= 300 && haveBusy ) VALID" );
		Sptr <UaStateMachine> stateMachine;
		stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
		assert( stateMachine != 0 );

		//added by lijie 2005-08-24, we should send an ack to far end
		AckMsg ack( *msg );
		Sptr <Contact> origContact = call->findContact( *msg );
		assert( origContact != 0 );
		Sptr <BaseUrl> baseUrl =	origContact->getInviteMsg().getRequestLine().getUrl();
		assert( baseUrl != 0 );
		if( baseUrl->getType() == TEL_URL )
		{
			cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
			assert( 0 );
		}
		// Assume we have a SIP_URL
		Sptr< SipUrl > reqUrl;
		reqUrl.dynamicCast( baseUrl );
		assert( reqUrl != 0 );

		SipRequestLine reqLine = ack.getRequestLine();
		reqLine.setUrl( reqUrl );
		ack.setRequestLine( reqLine );
		sipEvent->getSipStack()->sendAsync( ack );
		
		//added ends here
		cpDebug(LOG_DEBUG, "returning StateBusy");
		return stateMachine->findState( "StateBusy" );
	}
	
	return 0;
}

