/*
* $Id: VoipOpMsgInviteIdle.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "DialingValidator.hxx"
#include "PbxAgent.hxx"
#include "CallMsgBuilder.hxx"
#include <stdlib.h>

#include "VoipOpBuilder.hxx"
using namespace Assist;

void SavePrevCalls(const Sptr <PbxEvent> event, const Sptr<InviteMsg> sipMsg )
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

	PbxAgent::instance()->getGateway()->getDialingValidator()->addIncomingUrl(caller);
	
 }

static const double maxval = 4294967296.0-1.0;

const Sptr <PbxState> VoipOpMsgInviteIdle::process( const Sptr <PbxEvent> event )
{
	cpDebug(LOG_DEBUG, "OpRing operation" );

	if( getSipMsgType(event) != SIP_INVITE)
	{
		cpDebug(LOG_DEBUG, "Not an INVITE" );
		return 0;
	}

	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);
	Sptr <CallEvent> setupEvent = new CallEvent();
	Sptr <CommonMsg> setupMsg = new _CallSetup;
	setupMsg->setSignalType(dtmf_signal_out_band);
	
	Sptr <InviteMsg> inviteMsg ;
	inviteMsg.dynamicCast(isSipMsg(event) );
	assert(inviteMsg !=0 );

	/* added by wuaiwu, 2005/11/16 */
	StatusMsg status( *inviteMsg, SIP_STATUS_CODE_INFO_TRYING );
	vep->getSipStack()->sendReply(status);
#if 0
	if (!(msg->getTo().getTag().length()))
	{
		SipTo tto = msg->getTo();
		tto.setTag(status.getTo().getTag());
	}
#endif 
	/* end of added */

	Sptr <SipContext> context = vep->getSipContext();
	if( context == 0 )
	{
		vep->setSipContext(new SipContext);
		context = vep->getSipContext();
	}
	assert(context != 0);
	
	SipCallId sipCallId = inviteMsg->getCallId();
	vep->setSipCallId(sipCallId );
	
	if (!context->getAuthorization())
    	{
    		cpDebug(LOG_DEBUG, "Not Authorization.");
		return 0;
    	}
	cpDebug(LOG_DEBUG, "Ring" );

	SavePrevCalls(event, inviteMsg);

	context->setContactMsg(*inviteMsg);

	//save route info
	context->setCalleeRoute1List( inviteMsg->getrecordrouteList() );
	int numContact = inviteMsg->getNumContact();
	if ( numContact )
	{
		SipContact contact = inviteMsg->getContact( numContact - 1 );
		Sptr <SipRoute> route = new SipRoute;
		route->setUrl( contact.getUrl() );
		cpDebug(LOG_DEBUG, "Remote contact: %s", route->encode().logData() );
		context->addRoute1( route );
	}

	cpDebug(LOG_DEBUG, "Ring" );
	Sptr <SipUrl> sipUrl;
	Data callingNum;
	SipRequestLine reqLine;
	double itemp = 0;
	int statusNo = SIP_STATUS_CODE_SUCCESS_OK;
	const char *from;
	
	Sptr <BaseUrl> baseUrl = inviteMsg->getFrom().getUrl();
	assert( baseUrl != 0 );
	if (baseUrl == 0)
	{
		statusNo = SIP_STATUS_CODE_CERROR_BAD_REQUEST;
		cpDebug(LOG_DEBUG, "baseUrl == 0");
		goto MSGPASERERROR;
	}
	
	if( baseUrl->getType() == TEL_URL )
	{
		statusNo = SIP_STATUS_CODE_CERROR_BAD_REQUEST;
		cpDebug(LOG_DEBUG,  "baseUrl->getType() == TEL_URL");
		cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
//		assert( 0 );
		goto MSGPASERERROR;
	}

	sipUrl.dynamicCast( baseUrl );
	if (sipUrl == 0)
	{
		statusNo = SIP_STATUS_CODE_CERROR_BAD_REQUEST;
		goto MSGPASERERROR;
	}
	assert( sipUrl != 0 );
   
	//cpDebug(LOG_DEBUG, "%d : %u : %f", temp, tem, maxval);
	
	itemp = atof(inviteMsg->getCSeq().getCSeqData().logData());
	cout << "CSeq: " << itemp << endl;
	cpDebug(LOG_DEBUG, "CSeq: %f - %s", itemp, inviteMsg->getCSeq().getCSeqData().logData());
	if (itemp > maxval)
	{
		statusNo = SIP_STATUS_CODE_CERROR_BAD_REQUEST;
		goto MSGPASERERROR;
	}

	itemp = atof(inviteMsg->getExpires().getData().logData());
	cpDebug(LOG_DEBUG, "Expires: %f", itemp);
	if (itemp > maxval)
	{
		statusNo = SIP_STATUS_CODE_CERROR_BAD_REQUEST;
		cpDebug(LOG_DEBUG, "msg->getExpires().getData.convertLong() > maxCseq");
		goto MSGPASERERROR;
	}
  
	reqLine = inviteMsg->getRequestLine();
	baseUrl = reqLine.getUrl();
	assert( baseUrl != 0 );
	if (baseUrl == 0)
	{
		goto MSGPASERERROR;
	}
	if( baseUrl->getType() == TEL_URL )
	{
		cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
		assert( 0 );
//		goto MSGPASERERROR;
	}

	// Assume we have a SIP_URL
	sipUrl.dynamicCast( baseUrl );
	if (sipUrl == 0)
	{
		statusNo = 416;
		goto MSGPASERERROR;
	}
	assert( sipUrl != 0 );
	context->setRingInvite( new InviteMsg( *inviteMsg ) );
 

#if __CDR_OPEN__
	if (SimulateDB::instance() != NULL)
	{
		int deviceID = event->getDeviceId();
		SimulateDB::instance()->addCdrClient(inviteMsg, deviceID, CALL_RING);
	}
#endif

	if( vep->getCallId() == 0)
	{
		vep->setCallId(new CallId);
	}
	else
	{
		cpLog(LOG_WARNING, "CallId %s has exist for VOIP EndPoint : %s", vep->getCallId()->encode().logData(), vep->getName() );
	}
	
	assert( vep->getCallId() != 0);
	cpLog(LOG_DEBUG, "callid in VOIP EndPoint : %s", vep->getCallId()->encode().logData());

	vep->setPeerNumber( inviteMsg->getTo().getUser().logData() );
	setupMsg->setTo( vep->getPeerNumber().c_str() );
//	setupMsg->setFrom(pep->getPhoneNumber() );
	cpLog(LOG_DEBUG, "VOIP Caller ID : %s", inviteMsg->getFrom().getUser().logData()  );

	from = inviteMsg->getFrom().getUser().logData();
//	if( from== NULL || strlen(from)==0 )
	{
		from = PbxConfiguration::instance()->getCallId().c_str();
	}
	setupMsg->setFrom( from);

	setupEvent->setCallMsg(setupMsg);
	setupEvent->setEndPoint(vep );
	setupEvent->setCallId(vep->getCallId() );

	sendSchedulerEvent(setupEvent);

	return lookupEPState(event, VOIP_EP_STATE_RINGING );

MSGPASERERROR:
	
	StatusMsg statu( *inviteMsg, statusNo );
	vep->getSipStack()->sendReply(statu);

	vep->reset();
	return lookupEPState(event, VOIP_EP_STATE_IDLE );

}
		
