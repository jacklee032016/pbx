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

static const char* const LoadGenMonitor_cxx_Version =
    "$Id: LoadGenMonitor.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";


#include "global.h"
#include <cassert>
#include <stdio.h>

#include "LoadGenMonitor.hxx"
#include "UaConfiguration.hxx"
#include "Timer.h"

using namespace Vocal;


/// Global benchmark data
int numOfCalls;
int numOfCallsStarted;
double lenOfCalls;
double lenOfInvite;
double lenOfBye;
int numOfBusys;
int numOfInvite;
int numOfBye;
timeval inviteTime;
timeval inviteokTime;
timeval byeTime;
timeval byeokTime;


///
Sptr < LoadGenMonitor > LoadGenMonitor::loadGenMonitor = 0;


///
Sptr < LoadGenMonitor >
LoadGenMonitor::instance()
{
    if (loadGenMonitor == 0)
    {

        loadGenMonitor = new LoadGenMonitor();
        assert( loadGenMonitor != 0 );
    }
    return loadGenMonitor;
}



///
void
LoadGenMonitor::lgMain(int dbgMsgInterval)
{
    static char buf[1024];
    time(&endTime);
    double aveInviteOk;
    double aveByeOk;
    double aveLenOfCalls;

    if ((difftime(endTime, statTime) > dbgMsgInterval))
    {
        aveInviteOk = (lenOfInvite / numOfInvite) / 1000;
        aveByeOk = (lenOfBye / numOfBye) / 1000;
        aveLenOfCalls = (lenOfCalls / numOfCalls) / 1000;

        sprintf( buf, "Calls:%7.0d   Len:%9.2f ms  InvOK:%9.2f ms  ByeOK:%9.2f ms",
                 numOfCalls, aveLenOfCalls, aveInviteOk, aveByeOk);

        printf("\r%s", buf);
        fflush(stdout);

        memcpy(&statTime, &endTime, sizeof(endTime));
    };


    if (UaConfiguration::instance()->getNumOfCalls() != -1 &&
            UaConfiguration::instance()->getNumOfCalls() <= numOfCalls )
    {
        printf( "\nReached %d calls in %.0f seconds\n",
                UaConfiguration::instance()->getNumOfCalls(),
                difftime( endTime, startTime ) );
        sleep(2);
        exit(0);
    }
}


///
LoadGenMonitor::LoadGenMonitor()
{
    time(&startTime);
    statTime = startTime;
    numOfCalls = 0;
    numOfCallsStarted = 0;
    lenOfCalls = 0;
    lenOfInvite = 0;
    lenOfBye = 0;
    numOfBusys = 0;
    numOfInvite = 0;
    numOfBye = 0;

    printf( "\n\n" );
}    // LoadGenMonitor::LoadGenMonitor

///
LoadGenMonitor::~LoadGenMonitor()
{}    // LoadGenMonitor::~LoadGenMonitor
