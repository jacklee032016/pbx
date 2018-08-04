/*
 * $Id: DeviceCommandThread.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include <cassert>

#include "GatewayMgr.hxx"
#include "DeviceCommandThread.hxx"

using namespace Assist;

DeviceCommandThread::DeviceCommandThread( const Sptr <GatewayMgr> gateway  )
{
	myGatewayMgr = gateway;
}

DeviceCommandThread::~DeviceCommandThread()
{
}

void DeviceCommandThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), "DeviceCommandThread");
	
	while ( true )
	{
		myGatewayMgr->deviceThreadMainLoop();
		
		if ( isShutdown() == true) 
			return;
	}
}

