/*
 * $Id: OpFarEndError.cxx,v 1.3 2007/03/30 09:13:18 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>

#include "OpFarEndError.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"

using namespace Vocal;

OpFarEndError::OpFarEndError()
{}

OpFarEndError::~OpFarEndError()
{}

const char* const OpFarEndError::name() const
{
	return "OpFarEndError";
}

const Sptr <State> OpFarEndError::process( const Sptr <SipProxyEvent> event )
{
	cpDebug(LOG_DEBUG, "OpFarEndError operation" );

	Sptr < SipEvent > sipEvent;
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

	Sptr < UaCallInfo > call;
	call.dynamicCast( event->getCallInfo() );
	assert( call != 0 );

	int status;
	int best = 1000;
	bool haveError = false;

	ContactList::iterator iter = call->getContactList()->begin();
	while ( iter != call->getContactList()->end() )
	{
		status = (*iter)->getStatus();
		if ( status < best )
		{
			best = status;
		}
		// If status == 486, this event will be handled by OpFarEndBusy
#if 0		
		if ( status >= 400 && status != 486 )
#else			
		/* when authen is on by UAS(Far End), 401|407 will be on status list 
		Li Zhijie, 2007.03.30 */
		if ( status >= 400 && status != 486 
			&& status != 401 && status != 407 )
#endif			
		{
			haveError = true;
			cpDebug(LOG_DEBUG, "Status %d from farend", status );
		}
		iter++;
	}
	
	if ( best >= 300 && haveError )
	{
		UaDevice::instance()->releaseRtpPort( event->getDeviceId() );

		Sptr <UaStateMachine> stateMachine;
		stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
		assert( stateMachine != 0 );

		// send an ack
		AckMsg ack(*msg );

		Sptr < Contact > origContact = call->findContact( *msg );
		assert( origContact != 0 );
		Sptr< BaseUrl > baseUrl = origContact->getInviteMsg().getRequestLine().getUrl();
		assert( baseUrl != 0 );
		if( baseUrl->getType() == TEL_URL )
		{
			cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
			assert( 0 );
		}
		
		// Assume we have a SIP_URL
		Sptr<SipUrl> reqUrl;
		reqUrl.dynamicCast( baseUrl );
		assert( reqUrl != 0 );

		SipRequestLine reqLine = ack.getRequestLine();
		reqLine.setUrl( reqUrl );
		ack.setRequestLine( reqLine );
		sipEvent->getSipStack()->sendAsync( ack );

		return stateMachine->findState( "StateFastBusy" );
	}
	return 0;
}

