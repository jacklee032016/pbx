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


static const char* const ServiceApplicationTest_cxx_Version = 
    "$Id: ServiceApplicationTest.cxx,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $";


#include "ServiceApplication.hxx"
#include "Config.hxx"
#include "Random.hxx"
#include <string>
#include <iostream>
#include <iomanip>

using Vocal::Services::ServiceApplication;
using Vocal::Configuration::NameValueMap;
using Vocal::Configuration::NO_VALUE;
using Vocal::Application;
using Vocal::ReturnCode;
using Vocal::Random;
using Vocal::SUCCESS;

class TestServiceApplication : public ServiceApplication
{
    public:
        TestServiceApplication(bool s) 
            :   ServiceApplication(s)
        {
        }
        
    	ReturnCode  	run();
};


Application *	Application::create()
{
    Random  random;
    bool    singleThreaded = static_cast<bool>(random() % 2);
    
    return ( new TestServiceApplication(singleThreaded) );
}


int main(int argc, char ** argv, char ** arge)
{
    return ( Application::main(argc, argv, arge) );
}


ReturnCode 
TestServiceApplication::run()
{
    cout << "------------------------------------" << endl;
    
    NameValueMap opt = config().options();
    
    for (   NameValueMap::const_iterator i = opt.begin();
            i != opt.end();
            ++i
        )
    {
        cout << i->first;
        
        if ( i->second.type != NO_VALUE )
        {
            cout << "\t:\t";
            for (   list<string>::const_iterator j = i->second.value.begin();
                    j != i->second.value.end();
                    ++j
                )
            {
                if ( j == i->second.value.begin() )
                {
                    cout << *j;
                }
                else
                {
                    cout << ", " << *j;
                }
            }
        }
        cout << endl;
    }

    cout << "------------------------------------" << endl;

    return ( SUCCESS );
}
