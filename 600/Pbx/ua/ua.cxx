
/*$Id: ua.cxx,v 1.8 2007/06/18 11:11:17 lizhijie Exp $*/
#include "aslib.h"
#include "global.h"
#include "pbx_globals.h"
#include "assist_version.h"

#if 0
#include "UaCommandLine.hxx"
#endif

#include "Daemon.hxx"
#include "cpLog.h"
#include "UaConfiguration.hxx"
#include "UaBuilder.hxx"
#include "UserAgent.hxx"
#include <stdio.h>

#if defined(HAS_VOICEMAIL)
#include "HeartbeatTxThread.hxx"
#endif

#include <stdlib.h>

using namespace Vocal;

int main( const int argc, const char* argv[] )
{
	
	if (argc >= 2 && (argv[1][1] == 's' || argv[1][1] == 'S'))
	{
		printf("======== Use syslog ========\n");
		cpLogOpenSyslog();
	}

	as_lcd_clear(0);
	//version
	for (int i = 1; i < 7; ++i)
	{
		as_lcd_printf(i, 1, " ");
	}	
	msleep(50);
	char buf[1024];

	sprintf(buf, "%s (version: %s.%s.%s-%s %s %s)\r\n", AS_MODULENAME,  AS_VERSION_MAJOR, AS_VERSION_MINOR, AS_VERSION_RELEASE, AS_VERSION_MODIFY, __DATE__, __TIME__);
	as_ver_debug(buf);
	as_ver_log(AS_MODULENAME, buf);
	as_ver_opt(argc, const_cast<char**>(argv), buf);
	cpLogSetLabel( argv[0] );
	as_lcd_printf(2, 1, "System initializing...");
	msleep(50);
	as_lcd_time_update();
	msleep(50);
 
#if 0
	assert( Daemon() >= 0 );

	// Turn off retransmission
    if (UaCommandLine::instance( ) -> getIntOpt( "retransmit" ))
    {
        SipTransceiver::reTransOn();
    }
    else
    {
        SipTransceiver::reTransOff();
    }

    if (UaCommandLine::instance()->getIntOpt("retransinitial") != 500 ||
            UaCommandLine::instance()->getIntOpt("retransmax") != 4000 )
    {
        SipTransceiver::setRetransTime(
            UaCommandLine::instance()->getIntOpt("retransinitial"),
            UaCommandLine::instance()->getIntOpt("retransmax")
        );
    }
#endif

	cpLog( LOG_NOTICE, "Compiled at " __DATE__ " " __TIME__ );

#if 0
	const string cfgStr = IPPBX_CONFIG_FILE;

	FILE *cfgFile = fopen( cfgStr.c_str(), "r");
	if ( cfgFile == 0 )
	{
		cerr << "can not open " << cfgStr << endl;
		exit( 0 );
	}
	else
	{
		fclose( cfgFile );
		//UaConfiguration::instance( cfgStr );
	}
#endif 

	// if the config file has a log level, do something about it
	if(UaConfiguration::instance()->getLogFilename() != "")
	{
		int retval = cpLogOpen(UaConfiguration::instance()->getLogFilename().c_str());

		if(retval == 0)
		{
			cpLog(LOG_ALERT, "Could not open %s",   UaConfiguration::instance()->getLogFilename().c_str());
		}
	}

	cpLogSetPriority( UaConfiguration::instance()->getLogLevel() );

	/* added lizhijie, 2007.03.20 */
	cpLogSetFileSize(UaConfiguration::instance()->getLogSize() * 1024 );

	if(UaConfiguration::instance()->getLogLevel()==LOG_DEBUG)
		UaConfiguration::instance()->show();
	
	Sptr <UaBuilder> uaBuilder = new UaBuilder;

	UserAgent ua( uaBuilder, Data( UaConfiguration::instance()->getLocalSipPort() ).convertInt() );

	atexit(UserAgent::exit);

	ua.run();

#if defined(HAS_VOICEMAIL)
#if 0
    if ( UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
#endif		
    {
        cpDebug(LOG_DEBUG, " Inside voicemail");
        //create heartbeat transmit thread and start it.
        int sipPort = atoi(UaConfiguration::instance()->getLocalSipPort().c_str());
        cpDebug(LOG_DEBUG, " Local Sip port = %d" , sipPort);

#if 0
        if(!UaCommandLine::instance()->getBoolOpt("no_heartbeat"))
#endif			
        {
            Sptr < HeartbeatTxThread > heartbeatThread 
                = new HeartbeatTxThread(sipPort,
                                        500,
                                        (const char*)"226.2.2.5",
                                        6000);
            heartbeatThread->run();
            heartbeatThread->join();
        }
    }
#endif    
     
	//Add by chensir at 2005.12.01
	as_lcd_clear(0);
	as_lcd_printf(2, 3, "initialization OK");
	//Add by chensir
	msleep(50);
      	as_lcd_printf(4, 7, "Ready!");
		
	ua.join();
	return 0;
}    // ua main()

