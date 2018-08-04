/*
 * $Id: AudioThread.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include <cassert>

#include "GatewayMgr.hxx"
#include "AudioThread.hxx"

using namespace Assist;

AudioThread::AudioThread( const Sptr <GatewayMgr> gateway  )
{
	myGatewayMgr = gateway;
}

AudioThread::~AudioThread()
{
}

void AudioThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), "Audio Thread");
	
	cpLog(LOG_DEBUG, "PID of Audio Thread is %d" ,getpid() );

	while ( true )
	{
		myGatewayMgr->processAudio();
		
		if ( isShutdown() == true) 
			return;
	}
}

