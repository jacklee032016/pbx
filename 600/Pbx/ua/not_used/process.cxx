
/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

static const char* const process_cxx_Version =
    "$Id: process.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";
//
// pieces of code for building Operator::process()
// Do not compile this

#include "global.h"
#include "SipEvent.hxx"
#include "ByeMsg.hxx"
#include "InviteMsg.hxx"
#include "AckMsg.hxx"
#include "InfoMsg.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "UaHardwareEvent.hxx"
#include "UaCallContainer.hxx"



// Get callInfo
Sptr < UaCallInfo > call;
call.dynamicCast( event->getCallInfo() );
assert( call != 0 );

// Create hardware msg event
Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );


// A request msg
signal->type = HardwareAudioType;
struct HardwareAudioRequest* request = &(signal->signalOrRequest.request);
request->type = AudioStart;


// A signal msg
signal->type = HardwareSignalType;
signal->signalOrRequest.signal = DeviceSignalBusyStart;


// Send to hardware thread
UaDevice::getDeviceQueue()->add( signal );


// SIP message event
Sptr < SipEvent > sipEvent;
sipEvent.dynamicCast( event );
if ( sipEvent == 0 )
{
    return 0;
}
Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
assert( sipMsg != 0 );


// Dynamic cast to SIP status message
Sptr < StatusMsg > msg;
msg.dynamicCast( sipMsg );
if ( msg == 0 )
{
    return 0;
}


// Dynamic cast to INVITE message
Sptr < InviteMsg > msg;
msg.dynamicCast( sipMsg );
if ( msg == 0 )
{
    return 0;
}


// Dynamic cast to BYE message
Sptr < ByeMsg > msg;
msg.dynamicCast( sipMsg );
if ( msg == 0 )
{
    return 0;
}


// Dynamic cast to Device Event
Sptr < UaDeviceEvent > deviceEvent;
deviceEvent.dynamicCast( event );
if ( deviceEvent == 0 )
{
    return 0;
}


// Dynamic cast to Timer Event
Sptr < UaDigitTimerEvent > timerEvent;
timerEvent.dynamicCast( event );
if ( timerEvent == 0 )
{
    return 0;
}


// Get next state
Sptr < UaStateMachine > stateMachine;
stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
assert( stateMachine != 0 );

return stateMachine->findState( "StateIdle" );


// Use DigitCollector
UaDevice::instance()->getDigitCollector()->reset();


// Send SIP message
StatusMsg status( *msg, 200/*OK*/ );
sipEvent->getSipStack()->sendReply( status );

sipEvent->getSipStack()->sendAsync( msg );


// Add myself to the contact
SipUrl myUrl;
myUrl.setUser( UaConfiguration::instance()->getUserName() );
myUrl.setHost( Data( theSystem.gethostAddress() ) );
myUrl.setPort( atoi( UaConfiguration::instance()->getLocalSipPort().c_str() ) );
SipContact me;
me.setUrl( myUrl );
status.setNumContact( 0 );    // Clear
status.setContact( me );


// Go thru local contact list
Sptr < Contact > contact;
Sptr < ContactList > contacts = call->getContactList();
ContactList::iterator iter = contacts->begin();
while ( iter != contacts->end() )
{
    contact = (*iter);
    assert( contact != 0 );
    iter++;
}


// Get current contact
Sptr < Contact > contact = call->getContact();
assert( contact != 0 );


// Save INVITE
call->setRingInvite( new InviteMsg( msg ) );

Sptr < Contact > contact = new Contact( msg );
// Set as current contact
call->setContact( contact );
// Add to contact list
call->addContact( contact );


// Get call container
Sptr < UaCallContainer > calls;
calls.dynamicCast( event->getCallContainer() );
assert( calls != 0 );


// Free device
UaDevice::instance()->setCallId( 0 );
