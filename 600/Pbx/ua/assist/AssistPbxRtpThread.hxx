#ifndef __ASSIST_PBX_RTP_THREAD_H__
#define __ASSIST_PBX_RTP_THREAD_H__
/* 
$Id: AssistPbxRtpThread.hxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/

#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"
#include "AssistDevice.hxx"

namespace Vocal
{

class AssistPbxRtpThread : public ThreadIf
{
	public:
		AssistPbxRtpThread( const Sptr < ResGwDevice > hwDevice );
		virtual ~AssistPbxRtpThread();

	protected:
		virtual void thread();

		Sptr < ResGwDevice > device;
};
 
}

#endif
