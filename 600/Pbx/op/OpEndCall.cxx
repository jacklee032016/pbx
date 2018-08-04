/*
 * $Id: OpEndCall.cxx,v 1.5 2007/03/21 18:12:39 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "ByeMsg.hxx"
#include "StatusMsg.hxx"
#include "OpEndCall.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaCallContainer.hxx"
#include "SdpHandler.hxx"
#if 0
#include "UaCommandLine.hxx"  
#endif

#ifdef HAS_RSVPLIB
#include "SuaRsvp.hxx"
#endif

#ifndef NO_POLICY
#include "PepAgent.hxx"
#endif

#include "pbx_globals.h"

#if __CDR_OPEN__

#include "SimulateDB.hxx"
using namespace SDB;
#endif
using namespace Vocal;


OpEndCall::OpEndCall()
{
}


OpEndCall::~OpEndCall()
{
}


const char* const
OpEndCall::name() const
{
    return "OpEndCall";
}


const Sptr <State> OpEndCall::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpEndCall operation" );

	Sptr < SipEvent > sipEvent;
	sipEvent.dynamicCast( event );
	if ( sipEvent == 0 )
	{
		return 0;
	}
	Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
	assert( sipMsg != 0 );

	Sptr <ByeMsg> msg;
	msg.dynamicCast( sipMsg );
	if ( msg == 0 )
	{
		return 0;
	}

	int statusno = 200;

	Sptr < UaCallInfo > call;
	call.dynamicCast( event->getCallInfo() );
	assert( call != 0 );

	Sptr < Contact > curContact;
	Sptr < Contact > byeContact;

	curContact = call->getContact();
	assert( curContact != 0 );
	byeContact = call->findContact( *msg );

	if ( byeContact == 0 )
	{
		statusno = 481;
		cpDebug(LOG_DEBUG, "Bye message is not for a known contact" );
	}

	if ( *curContact != *byeContact )
	{
		statusno = 481;
		cpDebug(LOG_DEBUG, "Not current contact" );
	}

#if 0	/* SJ Phone Always send out BYE with seq=1, so comments following, lzj 2007.03.18 */
#if 0
	if (msg->getCSeq().getCSeq() <= call->getRingInvite()->getCSeq().getCSeq())
#else	
	/* 2007.03.09; for Call between Software Phone and PBX */
	if (msg->getCSeq().getCSeq() < call->getRingInvite()->getCSeq().getCSeq())
#endif		
	{
		cpLog(LOG_WARNING, "seq of BYE is %d, but seq of RingInvite is %d", 
			msg->getCSeq().getCSeq(),  call->getRingInvite()->getCSeq().getCSeq() );
		statusno = 400;
	}
#endif

	StatusMsg status( *msg,statusno );
	sipEvent->getSipStack()->sendReply( status );

	if (statusno != 200)
		return 0;

    // Show caller information
#if 0    
    if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
    {
        cout << endl << VTime::strftime("%y/%m/%d %H:%M:%S")
             << " Call end " << msg->getFrom().encode().logData() << endl;
    }
    else
#endif		
    {
        cpDebug(LOG_DEBUG, "Call end: %s", msg->getFrom().encode().logData());
    }

#ifdef HAS_RSVPLIB
    // RSVP stuff ported from ol OpEndCall.cxx
    if ( UaConfiguration::instance()->getRsvpOn() )
    {
        UaDevice::instance()->getSuaRsvpSender().close();
        UaDevice::instance()->getSuaRsvpReserv().close();
    }
#endif

#ifndef NO_POLICY
    if ( UaConfiguration::instance()->getUsePolicyServer() )
    {
        cpDebug(LOG_DEBUG,"Disable QoS handled by Marshal");
    }
#endif

    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
     assert( calls != 0 );
		 
    string nextState = "StateCallEnded";

    SipCallId callId = msg->getCallId();
    // do the following only if the call is on call waiting
    if ( UaDevice::instance()->isOnCallWaiting( callId, event->getDeviceId() ) )
    {
        // delete the call from call container
  
	calls->deleteCall( callId, event->getDeviceId() );
       UaDevice::instance()->removeCallWaitingId( callId,  event->getDeviceId() );
       nextState = "StateIdle";
#if 0
        if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
#endif		
        {
            //cout << endl << "no more calls on call waiting" << endl;
	     cpDebug(LOG_DEBUG, "\nno more calls on call waiting");
        }
    }
	
//added by lijie 2005-6-3 
	if(event->getDeviceId() >= VIRTUAL_DEVICES_EPOCH)
	{
		calls->deleteCall( callId ,event->getDeviceId());
		UaDevice::instance()->setCallId(0,  event->getDeviceId() );
		nextState = "StateIdle";
	}
//added ends here

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

#if __CDR_OPEN__  
	 /* added by wuaiwu 2005/5/10 */

	if (SimulateDB::instance() != NULL)
	{
		int deviceID = event->getDeviceId();
		SimulateDB::instance()->addCdrClient(msg, deviceID, CALL_END);
	}

	/* end of added */
#endif
    return stateMachine->findState( nextState );
}


