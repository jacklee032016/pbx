#ifndef __AUDIO_THREAD_HXX__
#define __AUDIO_THREAD_HXX__
/*
 * $Id: AudioThread.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"

namespace Assist
{
class DeviceMgr;

class AudioThread : public ThreadIf
{
	public:
		AudioThread( const Sptr <GatewayMgr > gateway );
		virtual ~AudioThread();

	protected:
		virtual void thread();
		
		Sptr <GatewayMgr> 		myGatewayMgr;
};
 
}

#endif

