#ifndef __SUBSCRIBE_THREAD_HXX_
#define __SUBSCRIBE_THREAD_HXX_

/*
* $Id: SubscribeThread.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <Fifo.h>
#include <Sptr.hxx>
#include <ThreadIf.hxx>

#include "SubscribeManager.hxx"

namespace Assist
{

class SubscribeThread : public ThreadIf
{
	public:
		SubscribeThread( const Sptr <SubscribeManager> subManager );
		virtual ~SubscribeThread();

	protected:
		virtual void thread();

		Sptr <SubscribeManager> 				mySubscribeMgr;
};
 
}

#endif

