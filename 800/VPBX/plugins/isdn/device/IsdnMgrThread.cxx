/*
 * $Id: IsdnMgrThread.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include <cassert>

#include "IsdnDeviceMgr.hxx"
#include "IsdnMgrThread.hxx"

using namespace Assist;

IsdnMgrThread::IsdnMgrThread( IsdnDeviceMgr *deviceMgr  )
{
	isdnDeviceMgr = deviceMgr;
}

IsdnMgrThread::~IsdnMgrThread()
{
}

void IsdnMgrThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), "IsdnMgr");

	cpLog(LOG_DEBUG, "PID of ISDN-MGR is %d" ,getpid() );
	while ( true )
	{
		isdnDeviceMgr->mgrThreadMainLoop();
		
		if ( isShutdown() == true) 
			return;
	}
}

