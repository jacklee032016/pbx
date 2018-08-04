/*
 * $Id: OpTimeout.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpTimeout.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"


///
using namespace Vocal;


OpTimeout::OpTimeout()
{}


OpTimeout::~OpTimeout()
{}


///

const char* const
OpTimeout::name() const
{
    return "OpTimeout";
}


///
const Sptr < State >
OpTimeout::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpTimeout operation" );

    if ( isTimeout(event) == false )
        return 0;

   //Modified by lijie
 #if 0  
    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );


    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

    int status;
	
#if 0	
    int status = contact->getStatus();

    if ( status == 180 || status == 183 )
    {
        cpDebug(LOG_DEBUG, "180/183 message has already been received");
        cpDebug(LOG_DEBUG, "Igore timeout event");
        return 0;
    }
#else
    ContactList::iterator iter = call->getContactList()->begin();
    while ( iter != call->getContactList()->end() )
    {
        status = (*iter)->getStatus();
     	 if(status == 180 || status == 183)
		 	return 0;
        iter++;
    }
#endif	
#endif

	UaDevice::instance()->releaseRtpPort( event->getDeviceId() );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    //cout << endl << "fail to connect to remote endpoint" << endl;

    return stateMachine->findState( "StateError" );
}
