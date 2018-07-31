#ifndef __ISDN_MGR_THREAD_HXX__
#define __ISDN_MGR_THREAD_HXX__
/*
 * $Id: IsdnMgrThread.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"

namespace Assist
{
class IsdnDeviceMgr;

/* send different device command to hardware device */
class IsdnMgrThread : public ThreadIf
{
	public:
		IsdnMgrThread( IsdnDeviceMgr  *deviceMgr );
		virtual ~IsdnMgrThread();

	protected:
		virtual void thread();
		
		IsdnDeviceMgr 			*isdnDeviceMgr;
};
 
}

#endif

