/*
 * $Id: OpReDirect.cxx,v 1.4 2007/03/16 19:33:56 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "AckMsg.hxx"
#include "OpReDirect.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "SipTransceiver.hxx"
#include "SipVia.hxx"

using namespace Vocal;

OpReDirect::OpReDirect()
{
}

OpReDirect::~OpReDirect()
{
}

const char* const OpReDirect::name() const
{
	return "OpReDirect";
}

const Sptr <State> OpReDirect::process( const Sptr <SipProxyEvent> event )
{
	cpDebug(LOG_DEBUG, "OpReDirect operation" );

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
	
	switch ( msg->getStatusLine().getStatusCode() )
	{
		case 300:     // Multiple Choices
		case 301:     // Moved Premanently
		case 302:     // Moved Temporary
		case 305:     // Use Proxy
		{
			cpDebug(LOG_DEBUG, "Redirect" );
		}
		break;
		
		default:
		{
			//added by lijie to update status code for msg and set current contact
			Sptr<Contact> findContact = call->findContact( *msg );
			findContact->update(*msg);

			call->setContact(findContact);
			//added ends here 
			return 0;
		}
	}


	Sptr <Contact> origContact = call->findContact( *msg );
	assert( origContact != 0 );

	// Ack the 3xx
	AckMsg ack( *msg );

	Sptr <BaseUrl> baseUrl = origContact->getInviteMsg().getRequestLine().getUrl();
	assert( baseUrl != 0 );
	if( baseUrl->getType() == TEL_URL )
	{
		cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
		assert( 0 );
	}
	
	// Assume we have a SIP_URL
	Sptr <SipUrl> reqUrl;
	reqUrl.dynamicCast( baseUrl );
	assert( reqUrl != 0 );

	SipRequestLine reqLine = ack.getRequestLine();
	reqLine.setUrl( reqUrl );
	ack.setRequestLine( reqLine );
	sipEvent->getSipStack()->sendAsync( ack );

	for ( int i = 0; i < msg->getNumContact(); i++ )
	{
		SipContact sipContact = msg->getContact( i );

		baseUrl = sipContact.getUrl();
		assert( baseUrl != 0 );
		if( baseUrl->getType() == TEL_URL )
		{
			cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
			assert( 0 );
		}
		
		// Assume we have a SIP_URL
		Sptr <SipUrl> newUrl;
		newUrl.dynamicCast( baseUrl );
		Data tprt = newUrl->getTransportParam();

		assert( newUrl != 0 );

		if (newUrl->getUserParam() == "phone")
		{
#if 0		
			string proxyServer = UaConfiguration::instance()->getProxyServer();
			string::size_type colonPos = proxyServer.find( ":" );
			newUrl->setHost(proxyServer.substr( 0, colonPos ));
			cpDebug(LOG_DEBUG, "newUrl->getHost() = %s", newUrl->getHost().logData() );
			if ( colonPos < string::npos )
			{
				newUrl->setPort(proxyServer.substr( proxyServer.rfind( ":" ) + 1 ) );
			}
			else
			{
				newUrl->setPort("5060");
			}
#else
			newUrl->setHost( UaConfiguration::instance()->getProxyServer() );
			newUrl->setPort( UaConfiguration::instance()->getProxyPort() );
#endif
			cpDebug(LOG_DEBUG, "newUrl->getPort() = %s", newUrl->getPort().logData() );
		}

		//cout << "Redirected to " << newUrl->encode().logData() << endl;
		cpDebug(LOG_DEBUG, "Redirected to %s", newUrl->encode().logData());

		// create the new invite
		InviteMsg inviteMsg( origContact->getInviteMsg(), newUrl );

		//Set the transport in VIA same as the transport in the request line
		SipVia via = inviteMsg.getVia(0); 

		if(tprt == "tcp")
		{
			via.setTransport("TCP");
		}
		else
		{
			via.setTransport("UDP");
		}
		inviteMsg.removeVia(0);
		inviteMsg.setVia(via, 0);

		//check it is not a loop
		bool isLoop = false;
		//TODO Fix this
#if 0
		// Right now, we don't handle multiple contacts in ReDirect.
		// We just take the first one.
		Sptr < ContactList > contacts = call->getContactList();
		ContactList::iterator iter = contacts->begin();
		while ( iter != contacts->end() )
		{
			if ( *contact == *(*iter) )
			{
			// found a loop
				isLoop = true;
				cpLog( LOG_WARNING, "Redirect formed a loop" );
			}
			iter++;
		}
#endif
		if ( !isLoop )
		{
			// Use the same Call Id in the new INVITE
			inviteMsg.setCallId( sipMsg->getCallId() );
			// Increment CSeq
			SipCSeq newCSeq = inviteMsg.getCSeq();
			int cseq = sipMsg->getCSeq().getCSeqData().convertInt();
			/**
			*There is some question as to whether we should give a
			* unique callId.  For now, we'll just increase the CSeq.
			*/
			newCSeq.setCSeq( ++cseq );
			inviteMsg.setCSeq( newCSeq );

			if( sipMsg->getNumDiversion() )
			{
				cpDebug(LOG_DEBUG, "Copy diversion list" );
				inviteMsg.copyDiversionList( *sipMsg );
			}
			sipEvent->getSipStack()->sendAsync( inviteMsg );

			// Create the new contact
			Sptr <Contact> contact = new Contact( inviteMsg );

			// Add this to the contact list
			call->addContact( contact );
			// Set this to the current contact
			call->setContact( contact );

			// Update the Ring INVITE message
			call->setRingInvite( new InviteMsg( inviteMsg ) );
		}
	}
	return 0;
}

