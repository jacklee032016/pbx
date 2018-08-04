/*
 * $Id: ServiceApplication.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "ServiceApplication.hxx"
#include "Config.hxx"
#include "Daemon.hxx"
#include "Random.hxx"
#include "Thread.hxx"
#include "MTServiceManager.hxx"
#include "STServiceManager.hxx"
#include "SignalHandler.hxx"
#include "SignalAction.hxx"
#include "PollFifo.hxx"
#include "VLog.hxx"
#include <cassert>
#include <csignal>


using namespace Vocal;
using Vocal::Services::ServiceApplication;
using Vocal::Configuration::Config;
using Vocal::Services::ServiceManager;
using Vocal::Services::MTServiceManager;
using Vocal::Services::STServiceManager;
using Vocal::Services::Event;
using Vocal::Threads::Thread;
using Vocal::Signals::SignalHandler;
using Vocal::Signals::SignalSet;
using Vocal::Signals::SignalAction;
using Vocal::Logging::VLog;


ServiceApplication::ServiceApplication(bool singleThreaded)
    :	myConfig(0),
    	myTerminateSignals(0),
    	myTerminateAction(0),
    	myFifo(0),
    	myServiceMgr(0),
        mySingleThreaded(singleThreaded)
{
}


ServiceApplication::~ServiceApplication()
{
}


ReturnCode
ServiceApplication::init(int argc, char ** argv, char **)
{
    myConfig = createConfig();
    assert( myConfig != 0 );
    
    int     fatalError = 1;
    
    if  (   myConfig->load(argc, argv) != SUCCESS 
        ||  postConfig() != SUCCESS
        )
    {
        return ( fatalError );
    }

    if ( myConfig->asDaemon() == true )
    {
        if ( Daemon(myConfig->daemonRedirectOutput()) != SUCCESS )
        {
    	    cerr << "Cannot start as daemon." << endl;
	    return ( fatalError );
        }

        // Update the pid.
        //
        myConfig->newPid();
    }

    Thread::init();
    SignalHandler::init();
    VLog::init(myConfig->logLevel(), logFile().c_str());

    const string    fn("ServiceApplication::init");
    VLog    	    log(fn);

    // Report to both the log and cout, since both have proved useful.
    //
    VNOTICE(log) << "\n" << *myConfig << VNOTICE_END(log);
    cout << *myConfig << endl;

    // Initialize random.
    //
    Random::init();
    
    // Set up signal handling.
    //
    const vector<int> &  termSignals = terminateSignals();

    myTerminateSignals = new SignalSet(termSignals);
	
    Thread::self()->signalHandler().block(*myTerminateSignals);

    myTerminateAction = new SignalAction(*myTerminateSignals);
    
    Thread::self()->signalHandler().add(*myTerminateAction);


    // Create the message fifo.
    //
    if ( !mySingleThreaded )
    {
	myFifo = new PollFifo< Sptr<Event> >;
    }

    // Create service manager
    //
    if ( mySingleThreaded )
    {
        myServiceMgr = new STServiceManager(myTerminateAction);
    }
    else
    {
        myServiceMgr = new MTServiceManager(*myFifo, myTerminateAction);
    }

    // Give the use a chance to initialize.
    //
    ReturnCode rc = userInit();    

    // Double check userInit didn't do anything too strange.
    //
    assert( myConfig != 0 );
    assert( myTerminateSignals != 0 );
    assert( myTerminateAction != 0 );
    assert( myServiceMgr != 0 );
    
    if ( rc == SUCCESS )
    {
        // Report initialization complete.
        //
        VINFO(log) << fn << ": Program initialized." << VINFO_END(log);
    }
    else
    {
        VWARN(log) << fn << ": Exiting, rc = " << rc << VWARN_END(log);
    }    		    
    return ( rc );
}


void	    	    
ServiceApplication::uninit()
{
    userUninit();
    
    delete myServiceMgr;        myServiceMgr = 0;
    delete myFifo;              myFifo = 0;
    delete myTerminateAction;   myTerminateAction = 0;
    delete myTerminateSignals;  myTerminateSignals = 0;
    delete myConfig;            myConfig = 0;

    {
        VLog	log;
    	VINFO(log) << "ServiceApplication::uninit: Program uninitialized." 
	    	   << VINFO_END(log);
    }

    VLog::uninit();
    SignalHandler::uninit();
    Thread::uninit();
}


ReturnCode  	    
ServiceApplication::run()
{
    const string    fn("ServiceApplication::run");
    VLog    	    log(fn);
    
    ReturnCode	    rc = SUCCESS;
    
    VINFO(log) << fn << ": Program start." << VINFO_END(log);

    rc = myServiceMgr->start();

    if	( rc == SUCCESS )
    {
        VINFO(log) << fn << ": Services started." << VINFO_END(log);

        rc = myServiceMgr->run();
        
        if ( rc == SUCCESS )
        {
    	    myServiceMgr->stop();

            VINFO(log) << fn << ": Services stopped." << VINFO_END(log);
        }
    }
    
    myServiceMgr->shutdown();

    VINFO(log) << fn << ": Services shutdown." << VINFO_END(log);
    VINFO(log) << fn << ": Program shutdown." << VINFO_END(log);

    return ( rc );
}


Config *    
ServiceApplication::createConfig()
{
    return ( new Config );
}
        

ReturnCode
ServiceApplication::postConfig()
{
    return ( SUCCESS );
}

        
const vector<int> & 
ServiceApplication::terminateSignals() const
{
    static  vector<int>     signals;

    #if !defined(WIN32)
    if ( signals.size() == 0 )
    {
        signals.push_back(SIGHUP);
        signals.push_back(SIGTERM);
        signals.push_back(SIGINT);
    }
    #endif // !defined(WIN32)

    return ( signals );
}


string
ServiceApplication::logFile() const
{
    assert( myConfig != 0 );
    string filename = myConfig->applicationName() + string(".log");
    return ( filename );
}


ReturnCode
ServiceApplication::userInit()
{
    return ( SUCCESS );
}


void        
ServiceApplication::userUninit()
{
}


Config &                    
ServiceApplication::config()
{
    assert(myConfig);

    return ( *myConfig );
}


const Config &                    
ServiceApplication::config() const
{
    assert(myConfig);

    return ( *myConfig );
}


SignalAction &
ServiceApplication::signalAction()
{
    assert(myTerminateAction);

    return ( *myTerminateAction );
}


FifoBase< Sptr<Event> > &   
ServiceApplication::fifo()
{
    assert(myFifo);

    return ( *myFifo );
}
        
        
ServiceManager &            
ServiceApplication::serviceManager()
{
    assert(myServiceMgr);

    return ( *myServiceMgr );
}


