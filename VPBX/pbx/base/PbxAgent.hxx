#ifndef __PBX_AGENT_HXX__
#define __PBX_AGENT_HXX__
/*
* $Id: PbxAgent.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <Sptr.hxx>
#include "CallScheduler.hxx"
#include "GatewayMgr.hxx"
#include "PbxManagerThread.hxx"
#include "AudioThread.hxx"
#include "TaskThread.hxx"

#include "PbxEvent.hxx"

namespace Assist
{

class PbxEvent;
class DeviceCommandThread;

class PbxAgent 
{
	public:
		static PbxAgent* instance();

		~PbxAgent();

		void run();
		void shutdown();
		void join();

		Sptr < Fifo < Sptr <PbxEvent> > > getSchedulerQueue()
		{
			return myScheduler->getQueue();
		};

		Sptr < Fifo < Sptr <PbxEvent> > > getGatewayQueue()
		{
			return myGateway->getQueue();
		};

		Sptr < Fifo < Sptr <PbxEvent> > > findDeviceQueue( int index)
		{
			return myGateway->findDeviceQueue( index);
		};

		Sptr <CallScheduler> getScheduler()
		{
			return myScheduler;
		};

		Sptr <GatewayMgr> getGateway()
		{
			return myGateway;
		};

	protected:
		Sptr <CallScheduler>        			myScheduler;
		Sptr <GatewayMgr>        			myGateway;

		Sptr <AudioThread>		 		myAudioThread;
		Sptr <DeviceCommandThread> 		myDeviceCommandThread;
		Sptr <TaskThread> 				myTaskThread;


	private:
		PbxAgent( );
		void init();

		int addPlugIn(const char *_pluginName,  const char *_libName );
		
		static PbxAgent 					*agent;
		
};
 
}

#endif

