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


static const char* const OpAutoEndRecCall_cxx_Version =
    "$Id: OpAutoEndRecCall.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";



#include "global.h"
#include <SipEvent.hxx>
#include <ByeMsg.hxx>

#include "OpAutoEndRecCall.hxx"
#include "UaCallContainer.hxx"
#include "UaStateMachine.hxx"

#include "UaConfiguration.hxx"
#include "LoadGenMonitor.hxx"


extern int numOfCalls;
extern double lenOfCalls;
extern double lenOfInvite;
extern double lenOfBye;
extern int numOfBusys;
extern int numOfInvite;
extern int numOfBye;
extern timeval inviteTime;
extern timeval inviteokTime;
extern timeval byeTime;
extern timeval byeokTime;


///
using namespace Vocal;


OpAutoEndRecCall::OpAutoEndRecCall()
{}


OpAutoEndRecCall::~OpAutoEndRecCall()
{}


///

const char* const
OpAutoEndRecCall::name() const
{
    return "OpAutoEndRecCall";
}


///
const Sptr < State >
OpAutoEndRecCall::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpAutoEndRecCall operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < ByeMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }
    cpDebug(LOG_DEBUG, "End Call" );

    // printf( "***** OpAutoEndRecCall operation *****\n" );
    // Remove call info from call container

    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
    assert( calls != 0 );
    calls->deleteCall( sipEvent->getSipCallLeg()->getCallId() , event->getDeviceId() );


    StatusMsg status( *msg, 200/*OK*/ );
    sipEvent->getSipStack()->sendReply( status );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    // Update stats


    if ( UaConfiguration::instance()->getMonitorMsgOn() )
    {
        numOfCalls ++;
    }

    return stateMachine->findState( "StateAutoIdle" );
}
