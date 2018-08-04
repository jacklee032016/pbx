/*
 * Copyright (c) 1999-2000, Vovida Networks, Inc 
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. Neither name of or "Vovida Networks" nor the names of its
 *    contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef OpAutoCalleeInCallTO_HXX
#define OpAutoCalleeInCallTO_HXX

/*********************************************************************
 
 $Id: OpAutoCalleeInCallTO.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 
 *********************************************************************
 
  Copyright 2000 Vovida Networks, Inc.  All Rights Reserved.
 
 *********************************************************************
 
 $Log: OpAutoCalleeInCallTO.hxx,v $
 Revision 1.1.1.1  2006/11/29 09:50:50  lizhijie
 AS600 Pbx

 Revision 1.1.1.1  2006/07/12 16:16:48  lizhijie
 rebuild 

 Revision 1.1.1.1  2005/03/14 07:38:05  lizhijie
 PBX program

 Revision 1.1  2005/02/23 05:11:40  lizhijie
 no message

 Revision 1.1.1.1  2005/02/16 04:51:09  lizhijie
 new pbx code

 Revision 1.1.1.1  2005/02/05 05:30:17  lizhijie
 pbx from UA of vocal

 Revision 1.9  2002/11/09 02:13:07  derekm
 Data changes

 Revision 1.8  2002/07/12 00:24:23  veer
 Made changes for Intel c++ compiler

 Revision 1.7  2002/03/19 21:40:15  jason
 put all sip/sipstack and sip/base code into the Vocal namespace

 Revision 1.6  2001/02/07 22:51:40  icahoon
 Sip base rewrite.
 See http://www.vovida.org/bugzilla/long_list.cgi?buglist=254

 Revision 1.5  2001/02/01 01:23:06  icahoon
 There is now only one Operator::process() method in the sip base.
 The SipProxyEvent no longer derives from Event. Instead the functionality
 had been merged and Event has been moved to util/transport since it is
 still being used by the h.323 translator. Every sip proxy that
 referenced Event was changed to reference SipProxyEvent.

 As well, there were minor changes so that everything would build on Solaris 8.
 Everything except the h.323 translator should at least compile under linux and
 solaris at this point.

 Revision 1.4  2000/11/13 09:11:15  zolli
 Change in base code (no support for SipProxyEvent) needed to be implemented
 in the ua.  All Operators are now derived from UaOperator.

 Revision 1.3  2000/11/02 19:57:34  veer
 Porting Solaris CC Thanks Jeff!! CC 6.0

 Revision 1.2  2000/09/20 08:00:23  cvs
 global format change

 Revision 1.1  2000/08/24 00:22:44  chok
 Add load generator restart timers
 
 Revision 1.5  2000/06/09 01:07:58  zolli
 Added BSD license.
 
 Revision 1.4  2000/06/01 02:36:16  chok
 Check in more transfer code.
 Currently not compiling due to RSVP stuff.
 
 
 ********************************************************************/

#include "UaOperator.hxx"
#include <SipProxyEvent.hxx>

namespace Vocal
{

class OpAutoCalleeInCallTO : public UaOperator
{
    public:
        ///
        OpAutoCalleeInCallTO();
        ///
        virtual ~OpAutoCalleeInCallTO();
        ///
        const char* const name() const;
        ///
        const Sptr < State > process( const Sptr < SipProxyEvent > event );
};
 
}

#endif
