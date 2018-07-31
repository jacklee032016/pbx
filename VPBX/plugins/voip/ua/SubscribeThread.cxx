#include "global.h"
#include <cassert>

#include "SubscribeThread.hxx"

using namespace Assist;

SubscribeThread::SubscribeThread( const Sptr <SubscribeManager> subManager )
{
	mySubscribeMgr = subManager;
}

SubscribeThread::~SubscribeThread()
{}

void SubscribeThread::thread()
{
	while ( true )
	{
		mySubscribeMgr->subscribeMain();
	}
}

