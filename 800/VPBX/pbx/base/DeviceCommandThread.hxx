#ifndef __DEVICE_COMMAND_THREAD_HXX__
#define __DEVICE_COMMAND_THREAD_HXX__
/*
 * $Id: DeviceCommandThread.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"

namespace Assist
{
class DeviceMgr;

/* send different device command to hardware device */
class DeviceCommandThread : public ThreadIf
{
	public:
		DeviceCommandThread( const Sptr <GatewayMgr > gateway );
		virtual ~DeviceCommandThread();

	protected:
		virtual void thread();
		
		Sptr <GatewayMgr> 		myGatewayMgr;
};
 
}

#endif

