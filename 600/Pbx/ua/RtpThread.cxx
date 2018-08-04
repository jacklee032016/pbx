/*
 * $Id: RtpThread.cxx,v 1.4 2007/06/18 11:11:17 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "RtpThread.hxx"

using namespace Vocal;

RtpThread::RtpThread( const Sptr < ResGwDevice > hwDevice )
{
    device = hwDevice;
}


RtpThread::~RtpThread()
{
}


void RtpThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), "RTP Thread");
	
	cpDebug(LOG_DEBUG, "PID of RtpThread is %d" ,getpid() );

	while ( true )
	{
		device->processRTP();

		if ( isShutdown() == true)
			return;
	}
}

