#ifndef __TASK_THREAD_HXX__
#define __TASK_THREAD_HXX__
/*
 * $Id: TaskThread.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"

namespace Assist
{

class GatewayMgr;

/* used to create temporal thread, eg. task */
class TaskThread : public ThreadIf
{
	public:
		TaskThread( const Sptr <GatewayMgr> _gateway );
		virtual ~TaskThread();

	protected:
		virtual void thread();
		
		Sptr <GatewayMgr> 		myGateway;
};
 
}

#endif

