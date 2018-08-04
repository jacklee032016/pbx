/*
 * $Id: TaskThread.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"

#include "TaskThread.hxx"
#include "GatewayMgr.hxx"

using namespace Assist;

TaskThread::TaskThread( const Sptr <GatewayMgr> _gateway  )
{
	myGateway = _gateway;
}

TaskThread::~TaskThread()
{
}

void TaskThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), "TaskThread");
	
	while ( true )
	{
		cpDebug(LOG_DEBUG, "TaskThread is running" );
		myGateway->taskMainLoop();
		
		if ( isShutdown() == true) 
			return;
	}
}

