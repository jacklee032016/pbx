/*
 * $Id: OpRing.cxx,v 1.4 2007/03/21 18:12:39 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "OpRing.hxx"
#include "UaHardwareEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "RsvpHandler.hxx"
#include "SystemInfo.hxx"
#include "UaCallContainer.hxx"
#include <stdlib.h>
#if 0
#include "UaCommandLine.hxx"  
#endif

#include "SipVia.hxx"

#ifndef NO_POLICY
#include "PepAgent.hxx"
#endif

#if __CDR_OPEN__

#include "SimulateDB.hxx"
using namespace SDB;

#endif

using namespace Vocal;

static const double maxval = 4294967296.0-1.0;

OpRing::OpRing()
{
}

OpRing::~OpRing()
{
}

const char* const OpRing::name() const
{
	return "OpRing";
}


const Sptr <State> OpRing::process( const Sptr <SipProxyEvent> event )
{
	cpDebug(LOG_DEBUG, "OpRing operation" );

	Sptr <SipEvent> sipEvent;
	sipEvent.dynamicCast( event );
	if ( sipEvent == 0 )
	{
		return 0;
	}
	Sptr <SipMsg> sipMsg = sipEvent->getSipMsg();
	///
	assert( sipMsg != 0 );

	Sptr <InviteMsg> msg;
	msg.dynamicCast( sipMsg );
	if ( msg == 0 )
	{
		cpDebug(LOG_DEBUG, "Not an INVITE" );
		return 0;
	}

	Sptr <UaCallInfo> call;
	call.dynamicCast( event->getCallInfo() );
	///
	assert( call != 0 );

	if (!call->getAuthorization())
	{
		cpDebug(LOG_DEBUG, "Not Authorization.");
		return 0;
	}

	/* added by wuaiwu, 2005/11/16 */
	StatusMsg status( *msg, 100 );
	sipEvent->getSipStack()->sendReply(status);
#if 0
	if (!(msg->getTo().getTag().length()))
	{
		SipTo tto = msg->getTo();
		tto.setTag(status.getTo().getTag());
	}
#endif 
	/* end of added */

//added by lijie
	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

	cpDebug(LOG_DEBUG, "Ring" );

	/* added by wuaiwu for return call, 2005/4/11 */
	SavePrevCalls(sipEvent,msg);
	/* end of added */

	call->setContactMsg(*msg);

	//save route info
	call->setCalleeRoute1List( msg->getrecordrouteList() );
	int numContact = msg->getNumContact();
	if ( numContact )
	{
		SipContact contact = msg->getContact( numContact - 1 );
		Sptr < SipRoute > route = new SipRoute;
		route->setUrl( contact.getUrl() );
		cpDebug(LOG_DEBUG, "Remote contact: %s", route->encode().logData() );
		call->addRoute1( route );
	}


	cpDebug(LOG_DEBUG, "Ring" );
	Sptr <SipUrl> sipUrl;
	Data callingNum;
	SipRequestLine reqLine;
	double itemp = 0;
	int statusNo = 400;
	
	Sptr< BaseUrl > baseUrl = msg->getFrom().getUrl();
	assert( baseUrl != 0 );

	if (baseUrl == 0)
	{
		//statusNo = 400;
		cpDebug(LOG_DEBUG, "baseUrl == 0");
		goto MSGPASERERROR;
	}

	if( baseUrl->getType() == TEL_URL )
	{
		//statusNo = 400;
		cpDebug(LOG_DEBUG,  "baseUrl->getType() == TEL_URL");
		goto MSGPASERERROR;
		cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
		assert( 0 );
	}

	sipUrl.dynamicCast( baseUrl );
	if (sipUrl == 0)
	{
		//statusNo = 400;
		goto MSGPASERERROR;
	}

	assert( sipUrl != 0 );
   
	//cpDebug(LOG_DEBUG, "%d : %u : %f", temp, tem, maxval);
	
	itemp = atof(msg->getCSeq().getCSeqData().logData());
#if 0	
	cout << "CSeq: " << itemp << endl;
#endif
	cpDebug(LOG_DEBUG, "CSeq: %f - %s", itemp, msg->getCSeq().getCSeqData().logData());

	if (itemp > maxval)
	{
		//statusNo = 400;
		goto MSGPASERERROR;
	}

	itemp = atof(msg->getExpires().getData().logData());
	cpDebug(LOG_DEBUG, "Expires: %f", itemp);

	if (itemp > maxval)
	{
		//statusNo = 400;
		cpDebug(LOG_DEBUG, "msg->getExpires().getData.convertLong() > maxCseq");
		goto MSGPASERERROR;
	}

  
	reqLine = msg->getRequestLine();
	baseUrl = reqLine.getUrl();
	assert( baseUrl != 0 );
	if (baseUrl == 0)
	{
		goto MSGPASERERROR;
	}
    
	if( baseUrl->getType() == TEL_URL )
	{
		goto MSGPASERERROR;
		cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
		assert( 0 );
	}
	cpDebug(LOG_DEBUG, "Ring" );
    // Assume we have a SIP_URL
	sipUrl.dynamicCast( baseUrl );
	
	if (sipUrl == 0)
	{
		statusNo = 416;
		goto MSGPASERERROR;
	}

	call->setRingInvite( new InviteMsg( *msg ) );

   	 assert( sipUrl != 0 );

	 call->setRingInvite( new InviteMsg( *msg ) );

	 

#if __CDR_OPEN__
	if (SimulateDB::instance() != NULL)
	{
		int deviceID = event->getDeviceId();
		SimulateDB::instance()->addCdrClient(msg, deviceID, CALL_RING);
	}
#endif

    return stateMachine->findState( "StateRinging" );


	/* added by wuaiwu, 2005/10/17 */
	 
MSGPASERERROR:
	
	StatusMsg statu( *msg, statusNo );
	sipEvent->getSipStack()->sendReply(statu);
	Sptr < UaCallContainer > calls;
	calls.dynamicCast( event->getCallContainer() );
	assert( calls != 0 );
	calls->deleteCall( msg->getCallId(), event->getDeviceId() );
	UaDevice::instance()->setCallId( 0 ,event->getDeviceId() );


	return stateMachine->findState("StateIdle");

	/* end of added */	
}

void  OpRing::SavePrevCalls(const Sptr < SipProxyEvent > event, const Sptr<InviteMsg> sipMsg )
{
	string caller = sipMsg->getFrom().getUrl()->encode().logData();
	if (caller.size() < 0)
	{
		cpDebug(LOG_DEBUG, "Not a vaild caller.");
		return;
	}
	string::size_type fpos = caller.find(";");
	if (fpos != string::npos)
	{
		caller = caller.substr(0, fpos);
	}
	cpDebug(LOG_DEBUG, "-------I have got from url: %s", caller.c_str());

	int deviceID = event->getDeviceId();
	UaDevice::instance()->getDigitCollector(deviceID)->addIncomingUrl(caller);
}

