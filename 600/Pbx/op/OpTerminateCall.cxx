/*
 * $Id: OpTerminateCall.cxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "global.h"

#include "ByeMsg.hxx"
#include "AckMsg.hxx"
#include "OpTerminateCall.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "UaStateMachine.hxx"
#include "UaCallContainer.hxx"
#include "UaConfiguration.hxx"
#include "ResGwDevice.hxx"
#include "SystemInfo.hxx"
#include "SdpHandler.hxx"
#if 0
#include "UaCommandLine.hxx"  
#endif
#include "SipVia.hxx"

#ifdef HAS_RSVPLIB
#include "SuaRsvp.hxx"
#endif

#ifndef NO_POLICY
#include "PepAgent.hxx"
#endif

#if __CDR_OPEN__

#include "SimulateDB.hxx"
using namespace SDB;

#endif

using namespace Vocal;


OpTerminateCall::OpTerminateCall()
{
}


OpTerminateCall::~OpTerminateCall()
{
}


const char* const
OpTerminateCall::name() const
{
    return "OpTerminateCall";
}


const Sptr < State >
OpTerminateCall::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpTerminateCall operation" );

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

    cpDebug(LOG_DEBUG, "Terminate Call" );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < Contact > contact = call->getContact();

    if ( contact != 0 )
    {
        SipVia sipVia;
        sipVia.setprotoVersion( "2.0" );
        sipVia.setPort( atoi( UaConfiguration::instance()
			      ->getLocalSipPort().c_str() ) );
        sipVia.setTransport( UaConfiguration::instance()->getSipTransport() );
       
        unsigned int cseq = contact->getCSeqNum();
        contact->setCSeqNum( ++cseq );

        Sptr < ByeMsg > bye;
	Data hip;
        if ( contact->getStatus() != 200 || (call->getAckMsg() != 0) )
        {
	    // we use the ACK to create the BYE because the
	    // INVITE may not contain the tag

	    Sptr< AckMsg > ack = call->getAckMsg();

            if ( ack != 0 )
            {
                cpDebug(LOG_DEBUG, "Sending BYE to caller" );
                bye = new ByeMsg( *ack );
                //set the Transport also 
                SipRequestLine& reqLine = bye->getMutableRequestLine();
		hip=reqLine.getHost();
                reqLine.setTransportParam(UaConfiguration::instance()->getSipTransport());

                /** Added proxy authorization for BYE from vmserver */   
//                bye->setProxyAuthorization( msg.getProxyAuthorization() );
            }
            else
            {
                cpLog( LOG_ERR , "No ACK msg to produce BYE" );

			/* added by lizhijie, for local device call, 2005.02.17 */
			Sptr< InviteMsg > invite = call->getRingInvite();
			bye = new ByeMsg( *invite );
			SipRequestLine& reqLine = bye->getMutableRequestLine();
			hip=reqLine.getHost();
			reqLine.setTransportParam(UaConfiguration::instance()->getSipTransport());
			/* end of added */
			
            }
        }
        else  // status == 200
        {
            const StatusMsg& msg = contact->getStatusMsg();
            if ( &msg != 0 )
            {
                cpDebug(LOG_DEBUG, "Sending BYE to callee" );
                bye = new ByeMsg( msg );
                //set the Transport also 
                SipRequestLine& reqLine = bye->getMutableRequestLine();
		Data tmph=reqLine.getHost();
		hip=theSystem.gethostAddress(&tmph);
                reqLine.setTransportParam(UaConfiguration::instance()->getSipTransport());
            }
            else
            {
                cpLog( LOG_ERR , "No status message for BYE" );
            }
        }

	if( bye == 0 )
	{
	    cpLog( LOG_ERR, "bye == 0, unable to send BYE msg" );
	    return 0;
	}
        bye->flushViaList();
        sipVia.setHost( hip );
        bye->setVia( sipVia, 0 );

        SipCSeq sipCSeq = bye->getCSeq();
        sipCSeq.setCSeq( cseq );
        bye->setCSeq( sipCSeq );

        vector < SipRoute* > routeList;
        // add the route info
	// We haven't saved route info from the ACK.  We just use
	// what was saved from the INVITE.  Is this ok?
        InviteMsg invMsg = contact->getInviteMsg();
        if ( *(call->getRingInvite()) == invMsg )
        {
            cpDebug(LOG_DEBUG, "adding route info for the first call" );
            routeList = call->getRoute1List();
        }
        else
        {
            cpDebug(LOG_DEBUG, "adding route info for the second call" );
            routeList = call->getRoute2List();
        }

 		cpDebug(LOG_DEBUG, "Test");
		
        if ( routeList.size() > 0 )
        {
            bye->setRouteList( routeList );
            //
            SipRoute siproute = bye->getRoute(0);
            bye->removeRoute(0);

            SipRequestLine reqLine = bye->getRequestLine();
            reqLine.setUrl( siproute.getUrl() );
            reqLine.setTransportParam(UaConfiguration::instance()->getSipTransport());
            bye->setRequestLine( reqLine );
        }

        deviceEvent->getSipStack()->sendAsync( *bye );
#if 0		
        if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
        {
            cout << endl
                 << VTime::strftime("%y/%m/%d %H:%M:%S")
                 << " Bye " << bye->getTo().encode().logData()
                 << endl;
        }
        else
#endif			
        {
            cpDebug(LOG_DEBUG, "Bye :%s", bye->getTo().encode().logData());
        }

#ifdef HAS_RSVPLIB
        // RSVP stuff ported from old OpTerminateCall.cxx
        if ( UaConfiguration::instance()->getRsvpOn() )
        {
            UaDevice::instance()->getSuaRsvpReserv().close();
            UaDevice::instance()->getSuaRsvpSender().close();
        }
#endif

#ifndef NO_POLICY
        if ( UaConfiguration::instance()->getUsePolicyServer() )
        {
            cpDebug(LOG_DEBUG,"Disble QoS handled in Marshal");
        }
#endif
#if __CDR_OPEN__
	 /* added by wuaiwu 2005/5/10 */

	if (SimulateDB::instance() != NULL)
	{
		int deviceID = event->getDeviceId();
		SimulateDB::instance()->addCdrClient(bye, deviceID, CALL_END);
	}

	/* end of added */
#endif
    }

    else
    {
        cpLog( LOG_ERR , "No current contact" );
    }

    	cpDebug(LOG_DEBUG, "Test");
    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    // Remove call info from call container
    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
    assert( calls != 0 );

	calls->deleteCall( *(UaDevice::instance()->getCallId( event->getDeviceId() )) ,event->getDeviceId() );
	
    // Clear call id in device
    	cpDebug(LOG_DEBUG, "Test");
	UaDevice::instance()->setCallId( 0 ,event->getDeviceId()  );

    // if there is a call on call waiting, send that call an onhook evnet
    Sptr < SipCallId > call2Id = UaDevice::instance()->getCallWaitingId(event->getDeviceId());
    if ( call2Id != 0 )
    {
        //if ( UaConfiguration::instance()->getCallWaitingOn() )
        if ( UaDevice::instance()->getCallWaitingOn(event->getDeviceId()) )
        {
            // send a message to the call on call waiting, notifying
            // it that the active call is completed
            Sptr < UaCallContainer > calls;
            calls.dynamicCast( event->getCallContainer() );
            assert( calls != 0 );

			Sptr < UaCallInfo > call2 = calls->findCall( *call2Id, event->getDeviceId() );
			
            if ( call2 == 0 )
            {
                cpLog( LOG_ERR, "No call info found for the call"
		       " on call waiting");
                return 0;
            }

		UaDevice::instance()->setCallId( call2Id ,event->getDeviceId()  );
		
            Sptr < Fifo < Sptr < SipProxyEvent > > > eventQ = deviceEvent->getFifo();
			
			Sptr < UaDeviceEvent > signal = new UaDeviceEvent( eventQ );
			signal->type = DeviceEventHookDown;
			//signal->callId = call2Id;
			signal->setDeviceId( event->getDeviceId() );
			
			eventQ->add( signal );
        }
    }

#if 0
    if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
#endif		

    {
        //cout << "Ready" << endl;
    }

    return stateMachine->findState( "StateIdle" );
}

