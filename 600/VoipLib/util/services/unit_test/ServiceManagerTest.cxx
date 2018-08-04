/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2001 Vovida Networks, Inc.  All rights reserved.
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


static const char* const ServiceManangerTest_cxx_Version = 
    "$Id: ServiceManagerTest.cxx,v 1.1.1.1 2006/11/29 09:28:02 lizhijie Exp $";


#include "ServiceApplication.hxx"
#include "ServiceManager.hxx"
#include "EventObserver.hxx"
#include "PollFifoService.hxx"
#include "Service.hxx"
#include "Random.hxx"
#include "TimeVal.hxx"
#include "VLog.hxx"
#include <iostream>
#include <iomanip>
#include "SystemException.hxx"


using namespace Vocal;
using namespace Vocal::Behavioral;
using namespace Vocal::Services;
using namespace Vocal::Logging;
using namespace Vocal::TimeAndDate;
using namespace std;


class TestObserver : public EventObserver< Sptr<Event> >
{
    private:

        Service                 &   myService;
        Random                      rand;
        u_int32_t                   count;

    public:

        TestObserver(Service & service)
            :   EventObserver< Sptr<Event> >(service.getEventSubject()),
                myService(service),
                count(0)
        {
            myService.subscribe(*this);
        }

        virtual ~TestObserver()
        {
            myService.unsubscribe(*this);
        }

    	virtual bool        interestedIn(const Sptr<Event> & event)
        {
            return ( event == 0 );
        }
        
    	virtual void        onEvent(Sptr<Event> event)
        {
            const string    fn("TestObserver::onEvent");
            VLog            log(fn);
            
            VINFO(log) << fn << ": count = " << ++count << VINFO_END(log);

	    cout << "." << flush;
            
            if ( count >= 10 || rand() % 30 == 0 )
            {
                VINFO(log) << fn << ": throwing exception." << VINFO_END(log);

		cout << endl;

                throw SystemException("Boom", __FILE__, __LINE__, 
			(count >= 10 ? SUCCESS : count), true);
            }
            
            TimeVal     oneSecond(1000);
            
            myService.getFifo().addDelay(event, oneSecond);
        }
        
        ostream &           writeTo(ostream & out) const
        {
            return ( out << "TestObserver" );
        }
};


class TestService : public PollFifoService
{
    private:

        TestObserver    *   myObserver;
        
    public:
    
        TestService()
            :   PollFifoService("TestService"),
                myObserver(0)
        {
            const string    fn("TestService::TestService");
            VLog            log(fn);

            myObserver = new TestObserver(*this);
                        
            VINFO(log) << fn << ": this = " << this << VINFO_END(log);
        }
        
        virtual  ~TestService()
        {
            const string    fn("TestService::~TestService");
            VLog            log(fn);
            
            delete myObserver;
            myObserver = 0;
            
            VINFO(log) << fn << ": this = " << this << VINFO_END(log);
        }
        
        ReturnCode      onStart()
        {
            const string    fn("TestService::onStart");
            VLog            log(fn);
  
            VINFO(log) << fn << ": starting." << VINFO_END(log);
            
            ReturnCode  rc = SUCCESS;
            
            Random  rand;
            
            if ( rand() % 5 == 0 )
            {
                // We are going to fail start up.
                //
                VINFO(log) << fn << ": start failed." << VINFO_END(log);

		cout << "Start failed." << endl;
                
                rc = !SUCCESS;    
            }
            else
            {
                // Kick off observer
                //
                Sptr<Event> nullEvent;
                getFifo().addDelay(nullEvent, TimeVal(1000));
            }
            
            return ( rc );
        }
        
        ReturnCode      onStop()
        {
            const string    fn("TestService::onStop");
            VLog            log(fn);
  
            VINFO(log) << fn << ": stopping." << VINFO_END(log);
            
            ReturnCode  rc = SUCCESS;
            
            Random  rand;
            
            if ( rand() % 5 == 0 )
            {
                // We are going to fail stop up.
                //
                VINFO(log) << fn << ": stop failed." << VINFO_END(log);

		cout << "Stop failed." << endl;
                
                rc = !SUCCESS;    
            }          
            
            return ( rc );
        }
        
        ReturnCode	onShutdown()
        {
            const string    fn("TestService::onShutdown");
            VLog            log(fn);
            
            VINFO(log) << fn << ": shutdown." << VINFO_END(log);

	    cout << "\nBye" << endl;

	    return ( SUCCESS );
        }


	void		onException()
	{
	    cout << "\nException thrown" << endl;

	    PollFifoService::onException();
	}


	void		onSignalCaught()
	{
	    cout << "\nOuch" << endl;

	    PollFifoService::onSignalCaught();
	}
};
        

    
class TestServiceApplication : public ServiceApplication
{
    private:

        TestService     *   myService;
        
    public:
        TestServiceApplication(bool s) 
            :   ServiceApplication(s)
        {
        }
        
        ~TestServiceApplication()
        {
        }
        
    private:
            
        ReturnCode  userInit()
        {
            const string    fn("TestServiceApplication::userInit");
            VLog            log(fn);
            
            myService = new TestService;
            
            serviceManager().manage(*myService);
            
            VINFO(log)  << fn << ": application initialized." 
                        << VINFO_END(log);
                        
            return ( SUCCESS );
        }


        void    userUninit()
        {
            const string    fn("TestServiceApplication::userUninit");
            VLog            log(fn);

            serviceManager().unmanage();
            
            delete myService;
            myService = 0;

            VINFO(log)  << fn << ": application uninitialized." 
                        << VINFO_END(log);
        }

};


Application *	Application::create()
{
    Random  random;
    bool    singleThreaded = static_cast<bool>(random() % 2);
    
    cout << ( singleThreaded ? "Signal" : "Mutli" ) << "threaded" << endl;

    return ( new TestServiceApplication(singleThreaded) );
}


int main(int argc, char ** argv, char ** arge)
{
    //unsigned int    seed = 2782583247U;
    //unsigned int    seed = 2783049802U;
    unsigned int    seed = 0;
    Random::init(seed);

    cout << "Seed: " << Random::seed() << endl;
        
    return ( Application::main(argc, argv, arge) );
}


