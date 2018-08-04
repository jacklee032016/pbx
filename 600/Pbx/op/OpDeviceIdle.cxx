/*
 * $Id: OpDeviceIdle.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "global.h"
#include "OpDeviceIdle.hxx"
#include "ResGwDevice.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "Sdp2Media.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "SdpHandler.hxx"
#include "RsvpHandler.hxx"
#include "pbx_globals.h"

using namespace Vocal;


using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::SdpRtpMapAttribute;


OpDeviceIdle::OpDeviceIdle()
{
}


OpDeviceIdle::~OpDeviceIdle()
{
}


const char* const
OpDeviceIdle::name() const
{
    return "OpDeviceIdle";
}


const Sptr < State >
OpDeviceIdle::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpDeviceIdle operation" );

    Sptr < UaDeviceEvent > deviceEvent;
	
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }

   
    if ( deviceEvent->type != DeviceEventIdle )
    {
        return 0;
    }

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < InviteMsg > msg = call->getRingInvite();
    assert( msg != 0 );

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;

	signal->setDeviceId( event->getDeviceId() );

     signal->signalOrRequest.signal = DeviceSignalRingStart;
    UaDevice::getDeviceQueue()->add( signal );


    cpDebug(LOG_DEBUG, "Ring" );
    Sptr<SipSdp> remoteSdp;
    remoteSdp.dynamicCast (msg->getContentData(0));
    bool ringbackTone = false;

    //constructing local SDP

    SipSdp localSdp;
    
    if ( remoteSdp != 0 )
    {
        localSdp = *remoteSdp;

	Data rh=localSdp.getAddress();
        Data host = theSystem.gethostAddress(&rh);

        if(UaConfiguration::instance()->getNATAddress() != "")
        {
            host = UaConfiguration::instance()->getNATAddress();
        }
        
		setStandardSdp(localSdp, host, UaDevice::instance()->getRtpPort( event->getDeviceId() ));
    }

    // get the status of the hardware available or not available or
    // available for call waiting only
    cpDebug(LOG_DEBUG, "Ring" );



    // for now support remote ringback only for the first call, not on
    // call waiting

    StatusMsg statusMsg;
	
    if (UaConfiguration::instance()->getProvideRingback() && (remoteSdp != 0) )
    {
        ringbackTone = true;
         StatusMsg status(*msg, 183);
        status.setContentData( &localSdp );
       // call->setLocalSdp( new SipSdp( localSdp ) );
        statusMsg = status;

       call->setRemoteSdp( new SipSdp( *remoteSdp ) );
        call->setLocalSdp( new SipSdp( localSdp ) );

        Sptr < SipSdp > local2Sdp = call->getLocalSdp();
        Sptr < SipSdp > remoteSdp = call->getRemoteSdp();

#ifndef NO_POLICY
        if (UaConfiguration::instance()->getUsePolicyServer() )
        {
            cpDebug(LOG_DEBUG,"Enable QoS handled in Marshal");
        }
#endif
	  setupRsvp(*local2Sdp, *remoteSdp);
    }
    else
    {
        // If ProvideRingback is off, then we send a 180 WITHOUT SDP
        StatusMsg status( *msg, 180 );
        statusMsg = status;
    }

//modified by lijie
#if 0
    if ( remoteSdp != 0 && UaConfiguration::instance()->getProvideRingback() )
    {
        // do policy / rsvp stuff

        call->setRemoteSdp( new SipSdp( *remoteSdp ) );
        call->setLocalSdp( new SipSdp( localSdp ) );

        Sptr < SipSdp > localSdp = call->getLocalSdp();
        Sptr < SipSdp > remoteSdp = call->getRemoteSdp();

#ifndef NO_POLICY
        if (UaConfiguration::instance()->getUsePolicyServer() )
        {
            cpDebug(LOG_DEBUG,"Enable QoS handled in Marshal");
        }
#endif

	setupRsvp(*localSdp, *remoteSdp);
    }
#endif

    // TODO Call log Show caller information
#if 0    
    if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
    {
        cout << endl << VTime::strftime("%y/%m/%d %H:%M:%S")
             << " Call " << msg->getFrom().encode().logData() << endl;
    }
    else
#endif		
    {
        cpDebug(LOG_DEBUG, "Call:%s", msg->getFrom().encode().logData());
    }




//add by lijie, do not send status msg	
#if 0
    cpDebug(LOG_DEBUG, "Ring" );
    sipEvent->getSipStack()->sendReply( statusMsg );
#endif

    //turn on remote ringback if required
    if ( ringbackTone )
    {
    	//modified by lijie 2005-06-03
	//sendRemoteRingback(*remoteSdp);
	sendRemoteRingback(*remoteSdp, event->getDeviceId());
    }

    call->setCalleeIdle(true);

    deviceEvent->getSipStack()->sendReply( statusMsg );

    return 0;
}

