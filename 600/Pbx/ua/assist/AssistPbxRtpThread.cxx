/* 
$Id: AssistPbxRtpThread.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "AssistPbxRtpThread.hxx"

using namespace Vocal;

AssistPbxRtpThread::AssistPbxRtpThread( const Sptr < ResGwDevice > hwDevice )
{
	device = hwDevice ;
}

AssistPbxRtpThread::~AssistPbxRtpThread()
{
}

void AssistPbxRtpThread::thread()
{
	cpLogSetLabelThread (VThread::selfId(), "PBX RTP Thread");

	cpDebug(LOG_DEBUG, "PID of AssistPbxThread is %d" ,getpid() );
	
	while ( true )
	{
		device->processPbxRTP();

		if ( isShutdown() == true) 
			return;
	}
}

