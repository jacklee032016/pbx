/*
 * $Id: UserAgent.cxx,v 1.6 2007/06/18 11:11:17 lizhijie Exp $
 */

#include "global.h"
#include "UserAgent.hxx"
#if 0
#include "UaCommandLine.hxx"
#endif
#include "UaConfiguration.hxx"
#include "UaDevice.hxx"
#include "DeviceThread.hxx"
#include "RtpThread.hxx"
#include "FeatureThread.hxx"
#include "UaCallContainer.hxx"

#if 0
#include "AssistCheckHardware.hxx"
#endif

#include "SubscribeManager.hxx"
#include "ResGwDevice.hxx"
#include "cpLog.h"

#include "UaDigitTimerEvent.hxx"
#if 0
#include "LoadGenMonitor.hxx"
#include "LoadGenThread.hxx"
#endif

#include "aslib.h"

using namespace Vocal;

//extern int numOfCallsStarted;

extern "C" 
{
	int cgi_signal_register_handler(int sigkey, void (*sig_handler)(int, siginfo_t *, void *));
	void sighandlerexit(int signo, siginfo_t *info, void *val);
}

void sighandlerexit(int signo, siginfo_t *info, void *val)
{
	UaDevice::LockDevice();
	UaDevice::setExitFlag(true);
	UaDevice::UnlockDevice();
}

int cgi_signal_register_handler(int sigkey, void (*sig_handler)(int, siginfo_t *, void *))
{
	struct sigaction act;

	sigemptyset(&act.sa_mask);
	act.sa_flags=SA_SIGINFO;
	act.sa_sigaction=sig_handler;
	return sigaction(sigkey, &act, NULL);
}
/*add by Tangbiao 2005-4-5*/


///
UserAgent::UserAgent( Sptr<UaBuilder> uaBuilder, unsigned short sipPort, Data appName )
	: HeartLessProxy( uaBuilder, sipPort, appName )
{
	const char* useDevice = "ASSIST";  // default to NULL_HARDWARE

#if 0
    if ( UaCommandLine::instance()->getBoolOpt( "quicknet" ) &&
         UaCommandLine::instance()->getBoolOpt( "voicemail" ) &&
         UaCommandLine::instance()->getBoolOpt( "soundcard" ) )
    {
        cpLog(LOG_ERR, "Quicknet and Soundcard and Voicemail mutually exclusive");
        exit( -1 );
    }

#ifdef HAS_PHONEDEV
    if ( UaCommandLine::instance()->getBoolOpt( "quicknet" ) )
    {
        // then create a CheckHardware object
        CheckHardware check;
        useDevice = check.GetType();
    }
    else
#endif
#ifdef HAS_SOUNDCARD
    if( UaCommandLine::instance()->getBoolOpt( "soundcard" ) )
    {
            useDevice = "SOUNDCARD";
    }
    else
#endif
#endif

#if 0
/* added by lizhijie 2005.02.04 */
	AssistCheckHardware check;
	useDevice = check.getType();
#endif


#ifdef HAS_VOICEMAIL 
#if 0
    if( UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
    {
            useDevice = "VOICEMAIL";
    }
#endif		
#endif

#if 0
    if(! UaConfiguration::instance()->getLoadGenOn())
#endif

    {
        // Create devices only if load gen is turned OFF.

		UaDevice::instance( useDevice, myCallProcessingQueue );
   
		cpDebug(LOG_DEBUG, "Create RTP Thread" );
		rtpThread = new RtpThread( UaDevice::instance() );
		assert( rtpThread != 0 );

		cpDebug(LOG_DEBUG, "Create Device Thread" );
		deviceThread = new DeviceThread( UaDevice::instance() );
		assert( deviceThread != 0 );

		cpDebug(LOG_DEBUG, "Create PBX RTP Thread" );
		pbxRtpThread = new AssistPbxRtpThread( UaDevice::instance() );
		assert( pbxRtpThread != 0 );

	  /* added by wuaiwu, 2005/5,12 */
	#if	__CDR_OPEN__
	
		if (UaConfiguration::instance()->getCdrOn())
		{
			cpDebug(LOG_DEBUG, "Create RecordThread in that time." );
			recordThread = new RecordThread;        
			assert (recordThread != 0);
		}
		else 
		{
			cpDebug(LOG_DEBUG, "CDR service not started!" );
			recordThread = 0;
		}
		
	#endif
		cpDebug(LOG_DEBUG, "Create SubscribeManager" );
		Sptr <SubscribeManager> subManager = new SubscribeManager( mySipStack );

		if ( UaConfiguration::instance()->getSubscribeOn() )
		{
			cpDebug(LOG_DEBUG, "Create Feature Thread" );
			featureThread = new FeatureThread( subManager );
			assert( featureThread != 0 );
			uaBuilder->setSubscribeManager( subManager );
		}
    }
	
#if 0	
    else
    {
        // Create monitor thread

        cpLog (LOG_DEBUG, "Load Generation turned on....  no devices created");
        if ( UaConfiguration::instance()->getMonitorMsgOn() )
        {
            // If load generation is turned on then create monitor thread
            // to capture statisics.
            cpDebug(LOG_INFO, "Creating Load Generation Monitor thread...");

            LoadGenMonitor::instance();
            loadGenThread = new LoadGenThread( LoadGenMonitor::instance() );
            assert( loadGenThread != 0 );

        }
        //create Rtp thread if Rtp is to be generated in load generation
        if ( ( UaConfiguration::instance()->getRtpGenOn() ) &&
	     ( strcmp(useDevice, "NULL") == 0) )
        {
            UaDevice::instance( useDevice, myCallProcessingQueue );

            cpDebug(LOG_INFO, "Create RTP Thread" );
            rtpThread = new RtpThread( UaDevice::instance() );
            assert( rtpThread != 0 );
            cpDebug(LOG_DEBUG, "Create Device Thread" );
            deviceThread = new DeviceThread( UaDevice::instance() );
            assert( deviceThread != 0 );
        }

    }
#endif

#if 0
    // Turn off retransmission
    if (UaCommandLine::instance( ) -> getBoolOpt( "retransmit" ) )
    {
        SipTransceiver::reTransOn();
    }
    else
    {
        SipTransceiver::reTransOff();
    }
#endif

	// Replace base CallContainer
	myCallContainer = new UaCallContainer;
	assert( myCallContainer != 0 );
	uaBuilder=uaBuilder;
	uaBuilder->setCallContainer( myCallContainer );
	uaBuilder->initCallContainer();

	uaBuilder->setSipStack( mySipStack );

	uaBuilder->startRegistration();

	/*add by Tangbiao 2005-4-5*/
	 cgi_signal_register_handler(CGI_TO_PBX_REBOOT, sighandlerexit); 
	/*add by Tangbiao 2005-4-5*/

	//cout << "Ready" << endl;
}    // UserAgent::UserAgent


///
void
UserAgent::run()
{
    HeartLessProxy::run();

    if (deviceThread != 0)
    {
        cpDebug(LOG_DEBUG, "Spawning Device Thread" );
        deviceThread->run();
    }


	if ( pbxRtpThread != 0 )
	{
		cpDebug(LOG_DEBUG, "Spawning PBX RTP Thread" );
		pbxRtpThread->run();
	}


    if ( rtpThread != 0 )
    {
        cpDebug(LOG_DEBUG, "Spawning RTP Thread" );
        rtpThread->run();
    }

    if ( featureThread != 0 )
    {
        cpDebug(LOG_DEBUG, "Spawning Feature Thread" );
        featureThread->run();
    }
#if __CDR_OPEN__

if ( recordThread != 0)
    {
    		cpDebug(LOG_DEBUG, "Spawning Record Thread");
		recordThread->run();
    }

#endif
#if 0
    if ( loadGenThread != 0 )
    {
        cpDebug(LOG_DEBUG, "Spawning Load Generation Monitor Thread");
        loadGenThread->run();
    }


    // User TimerEvent to kick start the load generator
    if ( UaConfiguration::instance()->getLoadGenOn() )
    {
        if (UaConfiguration::instance()->getRunMode() == "Calling" ||
                UaConfiguration::instance()->getRunMode() == "RSTest" )
        {
            printf("*****  Create kick start event *****\n");
            int numStarts = UaConfiguration::instance()->getNumKickStarts();
            if (numStarts < 1)
            {
                numStarts = 1;
            }
            int avgCallTime = 500;  // average length of one call is 500 ms
            int delay = avgCallTime / numStarts;  //kickstart all units in 500 ms
            for (int i = 0; i < numStarts; i++)
            {
                ++numOfCallsStarted;
                Sptr < UaDigitTimerEvent > kickStartEvent =
                    new UaDigitTimerEvent( myCallProcessingQueue );

                assert (kickStartEvent != 0);

                kickStartEvent->setLoadGenSignalType(LoadGenStartCall);
                kickStartEvent->setSipStack( mySipStack );

                myCallProcessingQueue->addDelayMs( kickStartEvent,
                                                    (UaConfiguration::instance()->getStartTime()) +
                                                    (delay * i));
            }
        }
    }
#endif	
}    // UserAgent::run



void UserAgent::join()
{
	HeartLessProxy::join();
}    // UserAgent::join

void UserAgent::exit()
{
	cpDebug(LOG_DEBUG, "None operation when performing UserAgent::exit()" );
}

UserAgent::~UserAgent()
{}

