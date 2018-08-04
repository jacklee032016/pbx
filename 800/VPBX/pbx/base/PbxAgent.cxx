/*
 * $Id: PbxAgent.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "vpbx_globals.h"
#include "cpLog.h"
#include "PbxAgent.hxx"
#include "GatewayMgr.hxx"
#include "CallScheduler.hxx"
#include "DeviceCommandThread.hxx"
#include "PbxConfiguration.hxx"

#include <dlfcn.h>

using namespace Assist;

PbxAgent* PbxAgent::agent = 0;

PbxAgent* PbxAgent::instance()
{
	if( agent == 0 )
	{
		agent = new PbxAgent;
		agent->init();
	}

	return agent;
}

PbxAgent::PbxAgent(  )
{
}

void PbxAgent::init()
{
//	Sptr <PbxAgent> pbx = PbxAgent::instance();
	
	cpLog( LOG_DEBUG, "Create CallScheduler" );
	myScheduler = new CallScheduler(this );
	assert( myScheduler != 0 );

	cpLog( LOG_DEBUG, "Create GatewayMgr" );

	myGateway = new GatewayMgr(this, myScheduler->getQueue() );
	assert(myGateway !=0 );
#ifndef API
	cpLog( LOG_DEBUG, "Create Audio Channel Thread" );
	myAudioThread = new AudioThread( myGateway );
	assert( myAudioThread != 0 );
#endif
	cpLog( LOG_DEBUG, "Create Device Command Thread" );
	myDeviceCommandThread = new DeviceCommandThread( myGateway );
	assert( myDeviceCommandThread != 0 );

	cpLog( LOG_DEBUG, "Create Task Thread" );
	myTaskThread = new TaskThread( myGateway );
	assert( myTaskThread != 0 );

//	as_watchdog_disable();
	cout << "Ready" << endl;
}

void PbxAgent::run()
{
	PlugInsMap plugIns = PbxConfiguration::instance()->getPlugIns();
	PlugInsMap::iterator plginIter = plugIns.begin();
	while ( plginIter != plugIns.end() )
	{
		addPlugIn( plginIter->first.c_str(),  plginIter->second.c_str() );
		plginIter++;
	}


	myScheduler->execute();
	myGateway->execute();
#ifndef API
	if ( myAudioThread != 0 )
	{
		myAudioThread->run();
	}
#endif
	if ( myDeviceCommandThread != 0 )
	{
		cpLog( LOG_DEBUG, "Spawning Device Command Thread" );
		myDeviceCommandThread->run();
	}

	if ( myTaskThread != 0 )
	{
		cpLog( LOG_DEBUG, "Spawning Task Thread" );
		myTaskThread->run();
	}

}

void PbxAgent::shutdown()
{
	myScheduler->shutdown();
}


void PbxAgent::join()
{
	/* suspend calling thread and waiting Scheduler terminated */
	cpLog( LOG_DEBUG, "Scheduler Thread join" );
	myScheduler->join();
#if 0
	cpLog( LOG_DEBUG, "Gateway Thread join" );
//	myGateway->join();
	cpLog( LOG_DEBUG, "Audio Thread join" );
//	myAudioThread->join();
	
	cpLog( LOG_DEBUG, "Device Command Thread join" );
//	myDeviceCommandThread->join();
	cpLog( LOG_DEBUG, "Task Thread join" );
//	myTaskThread->join();
#endif
}

PbxAgent::~PbxAgent()
{
}

int PbxAgent::addPlugIn(const char *_pluginName, const char *_libName)
{
	int ( *callBack)(const char *plugName, PbxAgent *agent );
	
	void *foo;
	void* handle = dlopen (_libName, RTLD_NOW);
	if(handle==NULL)
	{
		cpLog(LOG_ERR, "Dynamic Load %s(%s) Failed : %s",_pluginName, _libName, dlerror() );
		exit(1);
	}	

	foo = dlsym (handle, "init");
	if( foo ==NULL)
	{
		cpLog(LOG_ERR, "Symbol not found in Dynamic Load library %s(%s) : %s",_pluginName, _libName, dlerror() );
		exit(1);
	}
	callBack = (int (*)(const char *, PbxAgent *))foo;
	
	cpLog(LOG_DEBUG, "Init plugin %s ..... ", _pluginName );
	callBack(_pluginName, agent );

	return AS_OK;
}

