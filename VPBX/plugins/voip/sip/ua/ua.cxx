/* 
* $Id: ua.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "aslib.h"
#include "global.h"
#include "pbx_globals.h"
#include "assist_version.h"

#include "Daemon.hxx"
#include "cpLog.h"
#include "PbxConfiguration.hxx"
#include "UaBuilder.hxx"
#include "UserAgent.hxx"
#include <stdio.h>

#if defined(HAS_VOICEMAIL)
#include "HeartbeatTxThread.hxx"
#endif

#include <stdlib.h>

using namespace Assist;

int main( const int argc, const char* argv[] )
{
	if (argc >= 2 && (argv[1][1] == 's' || argv[1][1] == 'S'))
	{
		printf("======== Use syslog ========\n");
		cpLogOpenSyslog();
	}

	//Add by chensir at 2005.12.01
	as_lcd_clear(0);
	//version
	for (int i = 1; i < 7; ++i)
		as_lcd_printf(i, 1, " ");
	char buf[1024];

	sprintf(buf, "%s (version: %s.%s.%s-%s %s %s)\r\n", AS_MODULENAME,  AS_VERSION_MAJOR, AS_VERSION_MINOR, AS_VERSION_RELEASE, AS_VERSION_MODIFY, __DATE__, __TIME__);
	as_ver_debug(buf);
	as_ver_log(AS_MODULENAME,buf);
	as_ver_opt(argc, const_cast<char**>(argv), buf);
	cpLogSetLabel( argv[0] );
	as_lcd_printf(2, 1, "System initializing...");
	as_lcd_time_update();
	cpLog( LOG_NOTICE, "Compiled at " __DATE__ " " __TIME__ );

	// if the config file has a log level, do something about it
	if(PbxConfiguration::instance()->getLogFilename() != "")
	{
		int retval = cpLogOpen(PbxConfiguration::instance()->getLogFilename().c_str());

		if(retval == 0)
		{
			cpLog(LOG_ALERT, "Could not open %s",   PbxConfiguration::instance()->getLogFilename().c_str());
		}
	}

	cpLogSetPriority( PbxConfiguration::instance()->getLogLevel() );

	if(PbxConfiguration::instance()->getLogLevel()==LOG_DEBUG)
		PbxConfiguration::instance()->show();
	
	Sptr <UaBuilder> uaBuilder = new UaBuilder;

	UserAgent ua( uaBuilder, Data( PbxConfiguration::instance()->getLocalSipPort() ).convertInt() );

	atexit(UserAgent::exit);

	ua.run();

#if defined(HAS_VOICEMAIL)
	cpDebug(LOG_DEBUG, " Inside voicemail");
        //create heartbeat transmit thread and start it.
	int sipPort = atoi(PbxConfiguration::instance()->getLocalSipPort().c_str());
	cpDebug(LOG_DEBUG, " Local Sip port = %d" , sipPort);

	Sptr <HeartbeatTxThread> heartbeatThread = new HeartbeatTxThread(sipPort,
                                        500, (const char*)"226.2.2.5", 6000);
	heartbeatThread->run();
	heartbeatThread->join();

#endif    
     
	as_lcd_clear(0);
	as_lcd_printf(2, 3, "initialization OK");

	msleep(50);
      	as_lcd_printf(4, 7, "Ready!");
		
	ua.join();
	return 0;
}

