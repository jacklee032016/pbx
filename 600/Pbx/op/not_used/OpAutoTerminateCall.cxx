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


static const char* const OpAutoTerminateCall_cxx_Version =
    "$Id: OpAutoTerminateCall.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";


#include "global.h"
#include <ByeMsg.hxx>

#include "OpAutoTerminateCall.hxx"
//#include "UaDeviceEvent.hxx"
//#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "UaStateMachine.hxx"
#include "UaCallContainer.hxx"
#include "UaDigitTimerEvent.hxx"
#include "LoadGenMonitor.hxx"
#include "SystemInfo.hxx"
#include "SipVia.hxx"

extern void cjGetTimeOfDay( struct timeval* now );
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


OpAutoTerminateCall::OpAutoTerminateCall()
{}


OpAutoTerminateCall::~OpAutoTerminateCall()
{}


///

const char* const
OpAutoTerminateCall::name() const
{
    return "OpAutoTerminateCall";
}


///
const Sptr < State >
OpAutoTerminateCall::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpAutoTerminateCall operation" );

    Sptr < UaDigitTimerEvent > timerEvent;
    timerEvent.dynamicCast( event );

    if (timerEvent == 0)
    {
        return 0;
    }

    if (timerEvent->getLoadGenSigType() != LoadGenEndCall)
    {
        return 0;
    }


    // printf( "***** OpAutoTerminateCall operation *****\n" );

    cpDebug(LOG_DEBUG, "Terminate Call" );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < Contact > contact = call->getContact();
    if ( contact != 0 )
    {
        SipVia sipVia;
        sipVia.setprotoVersion( "2.0" );
        sipVia.setHost( Data( theSystem.gethostAddress() ) );
        sipVia.setPort( atoi( UaConfiguration::instance()->getLocalSipPort().c_str() ) );
        sipVia.setTransport( UaConfiguration::instance()->getSipTransport() );

        if ( contact->getStatus() != 200 )
        {
            const InviteMsg& msg = contact->getInviteMsg();
            
            cpDebug(LOG_DEBUG, "BYE to caller" );
            ByeMsg bye( msg );
            bye.flushViaList();
            bye.setVia( sipVia, 0 );
            timerEvent->getSipStack()->sendAsync( bye );
            //deviceEvent->getSipStack()->sendAsync( bye );

        }
        else
        {
            const StatusMsg& msg = contact->getStatusMsg();
           
            cpDebug(LOG_DEBUG, "BYE to callee" );
            ByeMsg bye( msg );
            bye.flushViaList();
            SipRequestLine& mLine = bye.getMutableRequestLine();
            mLine.setTransportParam( UaConfiguration::instance()->getSipTransport() );
            bye.setVia( sipVia, 0 );
            timerEvent->getSipStack()->sendAsync( bye );
            //deviceEvent->getSipStack()->sendAsync( bye );

        }
    }
    else
    {
        cpLog( LOG_ERR , "No current contact" );
    }


    // Update stats
    if ( UaConfiguration::instance()->getMonitorMsgOn() )
    {
        if (UaConfiguration::instance()->getCJTimeOfDay() )
        {
            cjGetTimeOfDay(&byeTime);
        }
        else
        {
            gettimeofday(&byeTime, NULL);
        }
    }

    return 0;
}
