#ifndef __PBX_MANAGER_THREAD_HXX__
#define __PBX_MANAGER_THREAD_HXX__
/*
 * $Id: PbxManagerThread.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */
#include "Fifo.h"
#include "Sptr.hxx"
#include "ThreadIf.hxx"

//#include "PbxManager.hxx"

namespace Assist
{
class PbxManager;

/* class for all Threads of PbxManager, such as ISDN/PSTN/SIP/Console, Device Write Thread, Scheduler Thread  */
class PbxManagerThread : public ThreadIf
{
	public:
		PbxManagerThread( const Sptr <PbxManager> manager );
		~PbxManagerThread();

		const char *getName() const;

	protected:
		virtual void thread();
		
		Sptr <PbxManager> 	myManager;

//		char 				myName[VPBX_NAME_LENGTH];
};
 
}

#endif

