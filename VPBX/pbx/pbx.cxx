/*
 * $Id: pbx.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "vpbx_globals.h"
#include <list>
#include <string>
//#include "assist_version.h"
#include <cassert>
#include <cstdio>

#if WITH_RELEASE
#include <iostream>
#endif

#include "Data.hxx"
#include "cpLog.h"

#include "Daemon.hxx"
#include "CallScheduler.hxx"
#include "PbxAgent.hxx"
#include <stdio.h>

#include "PbxConfiguration.hxx"
#include "assist_version.h"

using namespace Assist;

#if WITH_RELEASE
int wri_pid(int &proid)
{
	char filename[256]={0};
	FILE* fp;

	sprintf(filename,"/usr/bin/k_pbx");
	if ((fp=fopen(filename,"w+"))==NULL)
	{
		cout<<"open file "<<filename<<"error!"<<endl;
		return -1;
	}
	
	fprintf(fp,"kill -9 %d\n",proid);
	fclose(fp);
	sprintf(filename,"chmod +x /usr/bin/k_pbx");
	system(filename);
	return 1;
}
#endif

int main( int argc, char* argv[] )
{
/*
	as_ver_log(argc,  argv);
	as_ver_opt(argc, argv);
*/	
#if WITH_RELEASE
	int pid;
	pid=fork();

	switch(pid)
	{
		case 0:
			break;
		default:
			wri_pid(pid);
			cout<<"parent process exit!"<<endl;
			return 0;
	}
#endif
	
	cpLogSetLabel( argv[0] );
 
	cpLog( LOG_NOTICE, "Compiled at " __DATE__ " " __TIME__ );
	
	AS_VERSION_INFO(AS_PBX_MODULENAME)
#if 0
	assert( Daemon() >= 0 );
#endif


	const string cfgStr = ISDNPBX_CONFIG_FILE;

	FILE *cfgFile = fopen( cfgStr.c_str(), "r");
	if ( cfgFile == 0 )
	{
		cerr << "can not open " << cfgStr << endl;
		exit( 0 );
	}
	else
	{
		fclose( cfgFile );
		PbxConfiguration::instance( cfgStr );
	}

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

	PbxAgent::instance();
	PbxAgent::instance()->run();
	PbxAgent::instance()->join();

	return 0;
}

