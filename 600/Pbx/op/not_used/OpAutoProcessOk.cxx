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


static const char* const OpAutoProcessOk_cxx_Version =
    "$Id: OpAutoProcessOk.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";


#include "global.h"
#include <SipEvent.hxx>
#include <AckMsg.hxx>

#include "OpAutoProcessOk.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "UaDigitTimerEvent.hxx"

#include "UaCallContainer.hxx"
#include "LoadGenMonitor.hxx"
#include "Timer.h"

extern void cjGetTimeOfDay( struct timeval* now );
extern int numOfCalls;
extern int numOfCallsStarted;
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


OpAutoProcessOk::OpAutoProcessOk()
{}


OpAutoProcessOk::~OpAutoProcessOk()
{}


///

const char* const
OpAutoProcessOk::name() const
{
    return "OpAutoProcessOk";
}


///
const Sptr < State >
OpAutoProcessOk::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpAutoProcessOk operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < StatusMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }
    /*
        Sptr<UaCallInfo> call;
        call.dynamicCast( event->getCallInfo() );
        assert( call != 0 );
     
        Sptr<Contact> contact = call->findContact( *msg );
        assert( contact != 0 );
     
        contact->update( *msg );
        call->getContact()->update( *msg);
    */

    // printf( "***** OpAutoProcessOk operation *****\n" );

    int status = msg->getStatusLine().getStatusCode();

    if ( status >= 200 )
    {
        cpDebug(LOG_DEBUG, "***** Got 200 OK (start new call timer event) *****");
    }

    if ( status != 200 )
    {
        if ( status == 100 )
        {
            cpDebug(LOG_DEBUG, "*****  Discarding Status Msg 100 *****");
        }
        if (status == 180)
        {
            cpLog (LOG_DEBUG, "*****  Discarding Status Msg 180 *****");
        }

        return 0;  // Done
    }



    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );


    // Remove call info from call container
    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
    assert( calls != 0 );

    //calls->deleteCall( *(UaDevice::instance()->getCallId()) );

	calls->deleteCall( sipMsg->getCallId() , event->getDeviceId() );


    // Monitor msg:  Ok received for Bye
    if ( UaConfiguration::instance()->getMonitorMsgOn() )
    {
        if (UaConfiguration::instance()->getCJTimeOfDay() )
        {
            cjGetTimeOfDay(&byeokTime);
            lenOfBye += tv_diff( byeokTime, byeTime);
            lenOfCalls += tv_diff( byeokTime, inviteTime);
            numOfBye++;
        }
        else
        {
            gettimeofday(&byeokTime, NULL);
            lenOfBye += tv_diff( byeokTime, byeTime);
            lenOfCalls += tv_diff( byeokTime, inviteTime);
            numOfBye++;
        }

        numOfCalls ++;
    }


    if( UaConfiguration::instance()->getNumOfCalls() == -1 ||
        numOfCallsStarted < UaConfiguration::instance()->getNumOfCalls() )
    {
        ++numOfCallsStarted;
        //  Need to start another timer here for the next call
        Sptr < UaDigitTimerEvent > nextCallTimerEvent =
            new UaDigitTimerEvent( event->getFifo() );

        assert (nextCallTimerEvent != 0);

        nextCallTimerEvent->setLoadGenSignalType(LoadGenStartCall);
        nextCallTimerEvent->setSipStack( sipEvent->getSipStack() );

        (event->getFifo())->addDelayMs( nextCallTimerEvent,
                                        UaConfiguration::instance()->getCallDelay());
    }

    return stateMachine->findState( "StateAutoIdle" );
}
