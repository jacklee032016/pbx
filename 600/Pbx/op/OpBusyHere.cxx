/*
 * $Id: OpBusyHere.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "InviteMsg.hxx"
#include "OpBusyHere.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "SystemInfo.hxx"
#include "UaCallInfo.hxx"
#include "UaCallContainer.hxx"
#include "UaStateMachine.hxx"
#if 0
#include "UaCommandLine.hxx"  
#endif

///
using namespace Vocal;


OpBusyHere::OpBusyHere()
{
}


OpBusyHere::~OpBusyHere()
{}


///

const char* const
OpBusyHere::name() const
{
    return "OpBusyHere";
}


///
const Sptr < State >
OpBusyHere::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpBusyHere operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }

    /* modify by wuaiwu */
#if 0
    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );
     if(!call->getTimerSet())
     {
	 setTimer(event, 10); 
	 call->setTimerSet(true);
     }
#endif 
    /*end of modify */
		 
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < InviteMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

//modified by lijie, because no device is associated with. 2005-06-03
#if 0
    Sptr < SipCallId > currentCall = UaDevice::instance()->getCallId( event->getDeviceId() );
    if ( currentCall != 0 )
    {
        if ( msg->getCallId() == *currentCall )
        {
            // INVITE for the same call
            return 0;
        }
    }
#if 0	
    if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
    {
        cout << endl << "Busy - Cannot accept call " << msg->getFrom().encode().logData() << endl;
    }
    else
#endif	
#endif
//modified here
    {
        cpDebug(LOG_INFO, "Busy - Cannot accept call: %s" ,msg->getFrom().encode().logData());
    }

//#if 0
	/* added by wuaiwu */
      Sptr < UaCallContainer > calls;
      calls.dynamicCast( event->getCallContainer() );
      assert( calls != 0 );
      calls->deleteCall( msg->getCallId(), event->getDeviceId() );
      /* end of added */
//#endif

	StatusMsg status( *msg, 486 /*busy here*/ );
    // Add myself to the contact
	Sptr< SipUrl > myUrl = new SipUrl;
	myUrl->setUserValue( UaConfiguration::instance()->getUserName(), "phone" );
    myUrl->setHost(msg->getRequestLine().getHost());
	myUrl->setPort( UaConfiguration::instance()->getLocalSipPort() );
	SipContact me;
	me.setUrl( myUrl );
	status.setNumContact( 0 );    // Clear
	status.setContact( me );

	sipEvent->getSipStack()->sendReply(status); //, hosts ,  port);

	/* added by wuaiwu */
	Sptr < UaStateMachine > stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

      return stateMachine->findState( "StateIdle" );
	
}
